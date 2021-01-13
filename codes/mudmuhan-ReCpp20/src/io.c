/*
 * IO.C:
 *
 *	Socket input/output/establishment functions.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include "mstruct.h"
#include "mextern.h"

#define buflen(a,b,c)	(a-b + (a<b ? c:0))
#define saddr		addr.sin_addr.s_addr
#define MAXPLAYERS 256	

typedef struct wq_tag {
	int		fd;
	struct wq_tag	*next_tag;
} wq_tag;

int				Numplayers;
int				Numwaiting;
int				Deadchildren;
static wq_tag			*First_wait;
static int			Waitsock;
static fd_set			Sockets;
extern int			Port;

extern char title_cut_index[PMAX];

/**********************************************************************/
/*				sock_init			      */
/**********************************************************************/

/* This function initializes the socket that is used to accept new  */
/* connections on.						    */

void sock_init(port, debug)
int	port;
int	debug;
{
	struct sockaddr_in 	addr;
	struct linger		ling;
	int 			n, i;
	extern char		report;

	if(debug) {
		FD_SET(0, &Sockets);
		FD_SET(1, &Sockets);
		FD_SET(2, &Sockets);
	}
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGCHLD, child_died);

	Tablesize = getdtablesize();
	if (Tablesize > PMAX){
		Tablesize = PMAX;
		logf("Tablesize greater than PMAX\n");
	}
	Waitsock = socket(AF_INET, SOCK_STREAM, 0);
	if(Waitsock < 0)
		exit(-1);
	FD_SET(Waitsock, &Sockets);

	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (report){
   	i = 1;
        setsockopt(Waitsock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int));
	}
       
	n = bind(Waitsock, (struct sockaddr *) &addr, sizeof(addr));
	if(n < 0)
		exit(-1);

	ling.l_onoff = ling.l_linger = 0;
	setsockopt(Waitsock, SOL_SOCKET, SO_LINGER, (char *)&ling,
		   sizeof(struct linger));

	listen(Waitsock, 5);

	i=1;
	ioctl(Waitsock, FIONBIO, &i);
}

/**********************************************************************/
/*				sock_loop			      */
/**********************************************************************/

/* This function is the main loop of the entire program.  It constantly */
/* checks for new input, outputs players' output buffers, handles the   */
/* players' commands and updates the game.				*/

void sock_loop()
{
	while(1) {
		if(Deadchildren) reap_children();
		io_check();
		output_buf();
		handle_commands();
		update_game();
	}
}

/**********************************************************************/
/*				io_check			      */
/**********************************************************************/

/* This function takes a look at all the sockets that are being used at */
/* the time, and determines which ones have input waiting on them.  The */
/* ones that do call accept_input to have their input buffers updated.  */
/* If the wait socket indicates input is ready to read, that means a    */
/* new connection to the game must be accepted. 			*/

int io_check()
{
	fd_set			sockcheck;
	short			rtn=0, i;
	static struct timeval	t = {0L, 75000L};

	sockcheck = Sockets;
/*	t.tv_usec = T_USEC; */
/*	t.tv_sec = 0;	    */
	t.tv_usec = 75000L;
	if(select(Tablesize, &sockcheck, 0, 0, &t) > 0) {
		for(i=0; i<Tablesize; i++) {
			if(FD_ISSET(i, &sockcheck)) {
				if(i != Waitsock)
					rtn |= accept_input(i);
				else
					accept_connect();
			}
		}
	}

	return(rtn);
}

/**********************************************************************/
/*				accept_connect			      */
/**********************************************************************/

/* This function accepts a new connection on the wait socket so that a  */
/* new player can begin playing.  The player's iobuf structure is init- */
/* ialized and a spot is marked in the player and socket arrays for the */
/* player.								*/

void accept_connect()
{
	int			len, fd, i=1, pid;
	iobuf			*io;
	extra			*extr;
	struct linger		ling;
	struct sockaddr_in	addr;
	char			*inetname(), path[127], 
				port1str[10], port2str[10];

	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	addr.sin_addr.s_addr = INADDR_ANY;

	len = sizeof(struct sockaddr_in);
	fd = accept(Waitsock, (struct sockaddr *) &addr, &len);
	if (fd < 0)
		merror("accept_connect", FATAL);

	ioctl(fd, FIONBIO, &i);

	ling.l_onoff = ling.l_linger = 0;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&ling,
		   sizeof(struct linger));

	io = (iobuf *)malloc(sizeof(iobuf)+128);
	extr = (extra *)malloc(sizeof(extra)+128);
	if(!io || !extr) 
		merror("accept_connect", FATAL);

	Ply[fd].io = io;
	Ply[fd].ply = 0;
	Ply[fd].extr = extr;

	zero(extr, sizeof(extra));
	zero(io, sizeof(iobuf));
	io->ltime = time(0);
	io->intrpt = 1;

	strcpy(io->address, inetname(addr.sin_addr));

	pid = vfork();
	if(!pid) {
		sprintf(path, "%s/auth", BINPATH);
		sprintf(port1str, "%d", ntohs(addr.sin_port));
		sprintf(port2str, "%d", Port);
		execl(path, "auth", io->address, port1str, port2str, 0);
		exit(0);
	}
	else {
		strcpy(io->userid, "unknown");
		io->lookup_pid = pid;
	}

	FD_SET(fd, &Sockets);

	if(Numplayers > Tablesize-2) {
		print(fd, "Game full.  Try again later.\n");
		disconnect(fd);
		return;
	}

	else if(Numplayers >= MAXPLAYERS &&
		((unsigned)(ntohl(saddr))>>24) != 127) {
		if(Numwaiting > MAXPLAYERS) {
			write(fd, "Queue full.\n", 12);
			disconnect(fd);
			return;
		}
		add_wait(fd);
		RETURN(fd, waiting, 1);
	}

	init_connect(fd);
}

/************************************************************************/
/*				init_connect				*/
/************************************************************************/

/* This function sets up the player using the fd'th input socket.	*/

void init_connect(fd)
int	fd;
{
	int		i;

	/*************************************************************/
	/* The following lines must be left intact as part of the    */
	/* copyright agreement.					     */
	/*************************************************************/
	print(fd, "\n무한대전 1.3 (C) 1992, 1996");
	print(fd, "\n원작 MORDOR 2.5 (Brett J. Vickers)");
    print(fd, "\n관리자 테스토스 꽃 뽀뽀");
    /*************************************************************/
	/*************************************************************/

	Ply[fd].io->intrpt |= 2;
	Numplayers++;

	if((i = locked_out(fd)) > 1) {
		print(fd, "\nA password is required to play from that site.");
		print(fd, "\nPlease enter site password: ");
		output_buf();
		RETURN(fd, login, 0);
	}
	else if(i) {
		disconnect(fd);
		return;
	}

	Ply[fd].io->ltime = time(0);
	print(fd, "\n\n[엔터]를 누르세요. ");
	output_buf();
	RETURN(fd, login, -1);

}

/************************************************************************/
/*				locked_out				*/
/************************************************************************/

/* This function determines if the player on socket number, fd, is on	*/
/* a site that is being locked out.  If the site is password locked,	*/
/* then 2 is returned.  If it's completely locked, 1 is returned.  If	*/
/* it's not locked out at all, 0 is returned.				*/

int locked_out(fd)
int 	fd;
{
	int	i;

	for(i=0; i<Numlockedout; i++) {
		if(!addr_equal(Lockout[i].address, Ply[fd].io->address))
			continue;

		if(Lockout[i].password[0]) {
			strcpy(Ply[fd].extr->tempstr[0], Lockout[i].password);
			return 2;
		}
		else {
			write(fd, "\r\nYour site is locked out.\r\n", 28);
			return 1;
		}
	}

	return 0;
}

/************************************************************************/
/*				addr_equal				*/
/************************************************************************/

/* This function determines if two internet addresses are equal and	*/
/* allows for wild-cards.						*/

int addr_equal(str1, str2)
char	*str1;
char	*str2;
{

	while(*str1 && *str2) {
		if(*str1 == '*') {
			while(*str2 != '.' && *str2) str2++;
			str1++;
			continue;
		}
		else if(*str1 != *str2)
			return(0);
		str1++; str2++;
	}

	if(!*str1 && !*str2) return(1);
	else return(0);
}

/**********************************************************************/
/*				accept_input			      */
/**********************************************************************/

/* This function is called when a player's socket indicates that there */
/* is input waiting.  The socket is read from, and the input is copied */
/* into that player's input buffer.  If the last character entered is  */
/* a carriage return, then the player's interrupt flag is set high.    */

int accept_input(fd)
int	fd;
{
	char 	buf[128], lastchar;
	int 	i, n, prev, itail, ihead;

	n = read(fd, buf, 127);

	if(n<=0)
		Ply[fd].io->commands = -1;	/* Connection dropped */

	else {
		ihead = Ply[fd].io->ihead;
		lastchar = 0;
		itail = Ply[fd].io->itail;
		for(i=0; i<n; i++) {
			if(buf[i] > 31 || ((buf[i]&0x80) && (unsigned char)buf[i]!=155) || (buf[i] == '\n' && lastchar != '\r')
			    || buf[i] == '\b' ||buf[i]==127) {
                              if(buf[i]==127) buf[i]='\b';
				lastchar = buf[i];
				if(buf[i] == '\r') buf[i] = '\n';
				if(buf[i] == '\n') Ply[fd].io->commands++;
				else if(buf[i] == '\b' && ihead != itail) {
					prev = ihead-1 < 0 ? IBUFSIZE-1:ihead-1;
					/*
					if(Ply[fd].io->input[prev] == '%')
						ihead -= 2;
					else
					*/
						ihead--;
					if(ihead < 0)
						ihead = IBUFSIZE + ihead;
					continue;
				}
				else if(buf[i] == '\b') continue;
				Ply[fd].io->input[ihead] = buf[i];
				ihead = (ihead + 1) % IBUFSIZE;
				if(ihead == itail)
					itail = (itail + 1) % IBUFSIZE;
				/*
				if(buf[i] == '%') {
					Ply[fd].io->input[ihead] = buf[i];
					ihead = (ihead + 1) % IBUFSIZE;
					if(ihead == itail)
						itail = (itail + 1) % IBUFSIZE;
				}
				*/
			}
		}
		Ply[fd].io->ihead = ihead;
		Ply[fd].io->itail = itail;
		Ply[fd].io->ltime = time(0);
		if(buf[n-1] == '\n' || buf[n-1] == '\r')
			Ply[fd].io->intrpt |= 1;
		else
			Ply[fd].io->intrpt &= ~1;
	}

	return(0);
}

/**********************************************************************/
/*				output_buf			      */
/**********************************************************************/

/* This function outputs the contents of all players' buffers when that  */
/* player is able to be interrupted, or when that player's output buffer */
/* has reached a specific high-water mark (75% of buffer size).          */

void output_buf()
{
	char	str[128], *pstr;
	int 	i, n;
	int	otail, ohead;

	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].io) {
			otail = Ply[i].io->otail;
			ohead = Ply[i].io->ohead;
			if(ohead == otail)
				continue;
			if(Ply[i].io->commands == -1) {
				disconnect(i);
				continue;
			}
			if(Ply[i].io->intrpt & 1) {
				if(Ply[i].ply)
					if(Ply[i].ply->fd > -1 && 
					   F_ISSET(Ply[i].ply, PNOCMP)) {
						n = write(i, "\r\n", 2);
						if(Spy[i] > -1)
						    write(Spy[i], "\r\n", 2);
					}
				n = write(i, &Ply[i].io->output[otail], 
				 ohead>otail ? ohead-otail:OBUFSIZE-otail);
				if(Spy[i] > -1) write(Spy[i], 
				 &Ply[i].io->output[otail], 
				 ohead>otail ? ohead-otail:OBUFSIZE-otail);
				if(otail > ohead) {
					n+= write(i, Ply[i].io->output, ohead);
					if(Spy[i] > -1) 
					write(Spy[i], Ply[i].io->output, ohead);
				}
/*
				if(n < buflen(ohead, otail, OBUFSIZE))
					merror("output_buf", NONFATAL);
*/
				otail = ohead;
				Ply[i].io->otail = otail;
				if(Ply[i].ply) {
					pstr = ply_prompt(Ply[i].ply);
					n = write(i, pstr, strlen(pstr));
					if(Spy[i] > -1)
						write(Spy[i],pstr,strlen(pstr));
				}
			}
			if(buflen(ohead, otail, OBUFSIZE) > (OBUFSIZE*3)/4) {
				n = write(i, &Ply[i].io->output[otail], 
				 ohead>otail ? ohead-otail:OBUFSIZE-otail);
				if(Spy[i] > -1) write(Spy[i], 
				 &Ply[i].io->output[otail], 
				 ohead>otail ? ohead-otail:OBUFSIZE-otail);
				if(otail > ohead) {
					n+= write(i, Ply[i].io->output, ohead);
					if(Spy[i] > -1)
					write(Spy[i], Ply[i].io->output, ohead);
				}
/*
				if(n < buflen(ohead, otail, OBUFSIZE))
					merror("output_buf", NONFATAL);
*/
				otail = ohead;
				Ply[i].io->otail = otail;
			}
		}
	}
}

/**********************************************************************/
/*				print				      */
/**********************************************************************/

/* This function acts just like printf, except it outputs the     */
/* formatted text string to a given socket's output buffer.  The  */
/* socket number is the first parameter.			  */

void print(fd, fmt, i1, i2, i3, i4, i5, i6)
int 	fd;
unsigned char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char 	msg[3072];
	char	*fmt2;
	int	i = 0, j = 0, k, n, otail, ohead;
	int	num, loc, ind = -1, len, flags = 0;
	int	arg[8];
	char	type;
	char	*prestr;
	char	*Josa[6][2]={ {"은","는",},{"이","가",},{"과","와",},{"을","를",},{"으로","로",},};
      static char color_buf1[16];
      static char color_buf2[16];

	if(fd == -1 || fd > Tablesize)
		return;
	if(!Ply[fd].io)
		return;

	if(Ply[fd].ply) {
		if(F_ISSET(Ply[fd].ply, PDINVI))
			flags |= INV;
		if(F_ISSET(Ply[fd].ply, PDMAGI))
			flags |= MAG;
	        if(Ply[fd].ply->class==DM)
	                flags |= DMF;
	}

	len = strlen(fmt);
	fmt2 = (char *)malloc(len+1);
	if(!fmt2)
		merror("print", FATAL);

	arg[0] = i1; arg[1] = i2; arg[2] = i3; 
	arg[3] = i4; arg[4] = i5; arg[5] = i6;

	/* Check for %m, %M, %i and %I and modify arguments as necessary */
	do {
		if(fmt[i] == '%') {
			fmt2[j++] = fmt[i];
			num = 0; k = i;
			do {
				k++;
				if((fmt[k] >= 'a' && fmt[k] <= 'z') ||
				   (fmt[k] >= 'A' && fmt[k] <= 'Z') ||
				   fmt[k] == '%') {
					loc = k;
					type = fmt[k];
					break;
				}
				else if(fmt[k] >= '0' && fmt[k] <= '9')
					num = num*10 + fmt[k]-'0';
			} while(k < len);
			if(type == '%') {
				fmt2[j++] = '%';
				i++; i++;
				continue;
			}
			ind++;
			if(type != 'm' && type != 'M' && type != 'j' &&
			   type != 'i' && type != 'I' && type !='S' && type !='D' && type!='C') {
				i++;
				continue;
			}

			i = loc + 1;
			fmt2[j++] = 's';

			switch(type) {
			case 'm':
				arg[ind] = (int)(prestr=crt_str(arg[ind], num, flags|CAP));
				continue;
			case 'M':
				arg[ind] = (int)(prestr=crt_str(arg[ind], num, flags));
				continue;
			case 'S':
				prestr=(char *)arg[ind];
				continue;
			case 'j':
				switch(*((char *)arg[ind])) {
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
						if(under_han(prestr)) arg[ind]=(int)Josa[*((char *)arg[ind])-'0'][0];
						else arg[ind] = (int)Josa[*((char *)arg[ind])-'0'][1];
						break;
					default: arg[ind]= (int)"";
				}
				continue;
			case 'i':
			case 'I':
				arg[ind] = (int)(prestr=obj_str(arg[ind], num, flags));
				continue;
                         case 'C':
                              if(F_ISSET(Ply[fd].ply,PANSIC)) {
                                  sprintf(color_buf1,"%c[%d;%sm",27,
(F_ISSET(Ply[fd].ply,PBRIGH)&&atoi((char *)arg[ind])!=WHITE)?1:0,
(char *)arg[ind]);
                                  arg[ind]=(int)color_buf1;
                              } else arg[ind]= (int)"";
                              continue;
                         case 'D':
                              if(F_ISSET(Ply[fd].ply,PANSIC)) {
                                  sprintf(color_buf2,"%c[%d;%sm",27,
(F_ISSET(Ply[fd].ply,PBRIGH)&&atoi((char *)arg[ind])!=WHITE)?1:0,
(char *)arg[ind]);
                                  arg[ind]=(int)color_buf2;
                            } else arg[ind]= (int)"";
                            continue;
			}
		}
		fmt2[j++] = fmt[i++];
	} while (i < len);

	fmt2[j] = 0;

	sprintf(msg, fmt2, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
	free(fmt2);
	n = strlen(msg);
	if(n > 78 && title_cut_index[fd]==0) {
		delimit(msg);
		n = strlen(msg);
	}

	ohead = Ply[fd].io->ohead;
	otail = Ply[fd].io->otail;

	for(i=0; i<n; i++) {
		Ply[fd].io->output[ohead] = msg[i];
		if(msg[i]=='\n') {
			Ply[fd].io->output[ohead] = '\r';
			ohead = (ohead + 1) % OBUFSIZE;
			if(ohead == otail)
				otail = (otail + 1) % OBUFSIZE;
			Ply[fd].io->output[ohead] = msg[i];
		}
		ohead = (ohead + 1) % OBUFSIZE;
		if(ohead == otail)
			otail = (otail + 1) % OBUFSIZE;
	}
	Ply[fd].io->ohead = ohead;
	Ply[fd].io->otail = otail;
}

/**********************************************************************/
/*				handle_commands			      */
/**********************************************************************/

/* This function strips out the first command in each player's input */
/* buffer, and then sends that command to the player's next function */
/* of input with the appropriate parameter.			     */

extern char alias_buf[PMAX][MAX_ALIAS_BUF][256];
extern int  alias_buf_pos[PMAX];
extern int  alias_buf_num[PMAX];

int Write_CMD = 0;

void handle_commands()
{
    cmd cmnd;
    char commands[256];
    creature *ply_ptr;

	int	i, j;
	int	itail, ihead;
	char	buf[IBUFSIZE+1];

	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].io) {
			if(Ply[i].io->commands == -1) {
				disconnect(i);
				continue;
			}
        /* 줄임말 실행 */
        if(alias_buf_num[i]!=alias_buf_pos[i] && Ply[i].io->fn==command) {
            ply_ptr = Ply[i].ply;
            strcpy(commands,alias_buf[i][alias_buf_pos[i]++]);
            strcpy(cmnd.fullstr,commands);
            parse(commands, &cmnd);
            if(cmnd.num) {
                print(i,"\n");
                process_cmd(i, &cmnd);
            }
            continue;
        }

			if(!Ply[i].io->commands) continue;
			itail = Ply[i].io->itail;
			ihead = Ply[i].io->ihead;
			if(itail == ihead) continue;
			for(j=0; j<IBUFSIZE; j++) {
				if(itail == ihead) {
					buf[j] = 0;
					break;
				}
				if(Ply[i].io->input[itail] == 13 ||
				   Ply[i].io->input[itail] == 10) {
					itail = (itail + 1) % IBUFSIZE;
					buf[j] = 0;
					break;
				}
				buf[j] = Ply[i].io->input[itail];
				itail = (itail + 1) % IBUFSIZE;
			}
			Ply[i].io->itail = itail;
			Ply[i].io->commands--;
			if(Spy[i] > -1) {
				write(Spy[i], buf, strlen(buf));
				write(Spy[i], "\r\n", 2);
			}

			if(Write_CMD) log_overwrite("command.log","%s:%s\n",Ply[i].ply->name, buf);

			(*Ply[i].io->fn) (i, Ply[i].io->fnparam, 
        ((unsigned char)buf[0]==255 && 
        ((unsigned char)buf[1]==253 || (unsigned char)buf[1]==254)) ? buf+2 : buf);
		}
	}
}

/**********************************************************************/
/*				disconnect			      */
/**********************************************************************/

/* This function drops the connection to the player on the socket specified */
/* by the first parameter, clears his spot in the socket bit-array, and     */
/* removes him from the player array by freeing all memory taken by him.    */

void disconnect(fd)
int 	fd;
{
	int 	i;
	etag	*ign, *temp;
	wq_tag	*wq;

	close_alias(fd);
	close(fd);
	FD_CLR(fd, &Sockets);

	Spy[fd] = -1;

	if(Ply[fd].io) {
		if(Ply[fd].io->intrpt & 2)
			Numplayers--;
		free(Ply[fd].io);
		Ply[fd].io = 0;
	}
	if(Ply[fd].extr) {
		ign = Ply[fd].extr->first_ignore;
		while(ign) {
			temp = ign;
			ign = ign->next_tag;
			free(temp);
		}
		free(Ply[fd].extr);
		Ply[fd].extr = 0;
	}
	if(Ply[fd].ply) {
		if(F_ISSET(Ply[fd].ply, PSPYON)) {
			for(i=0; i<Tablesize; i++)
				if(Spy[i] == fd) Spy[i] = -1;
			F_CLR(Ply[fd].ply, PSPYON);
		}
		if(Ply[fd].ply->fd > -1) {
			uninit_ply(Ply[fd].ply);
			save_ply(Ply[fd].ply->name, Ply[fd].ply);
		}
		free_crt(Ply[fd].ply);
		Ply[fd].ply = 0;
	}
	else {
		for(wq = First_wait, i=1; wq; wq = wq->next_tag, i++)
			if(wq->fd == fd) {
				remove_wait(i);
				break;
			}
	}
	if(Numwaiting && Numplayers < MAXPLAYERS) {
		i = remove_wait(1);
		if ( i != -1){
		print(i, "%c", 7);
		init_connect(i);
		}
	}
}

/**********************************************************************/
/*				broadcast			      */
/**********************************************************************/

/* This function broadcasts a message to all the players that are in the */
/* game.  If they have the NO-BROADCAST flag set, then they will not see */
/* it.									 */

void broadcast(fmt, i1, i2, i3, i4, i5, i6)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n"); */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(!F_ISSET(Ply[i].ply, PNOBRD) && Ply[i].ply->fd > -1)
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

void broadcast2(fmt, i1, i2, i3, i4, i5, i6)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(!F_ISSET(Ply[i].ply, PNOBR2) && Ply[i].ply->fd > -1)
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

void broadcast_all(fmt, i1, i2, i3, i4, i5, i6)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n"); */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->fd > -1)
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

/**********************************************************************/
/*				broadcast_wiz			      */
/**********************************************************************/

/* This function broadcasts a message to all the DM's who are on at the */
/* time.								*/

void broadcast_wiz(fmt, i1, i2, i3, i4, i5, i6)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n");   */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->fd > -1 && Ply[i].ply->class >= CARETAKER){
				ANSI(i,YELLOW);
				print(i, fmt2, i1, i2, i3, i4, i5, i6); 
				ANSI(i,WHITE);
			}
	}
}

/**********************************************************************/
/*				broadcast_eaves			      */
/**********************************************************************/

/* This function broadcasts a message to all the DM's who are on at the */
/* time and have the eavesdropping flag set.			 	*/

void broadcast_eaves(fmt, i1, i2, i3, i4, i5, i6)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n"); */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->fd > -1 &&
			   Ply[i].ply->class > CARETAKER && 
			   F_ISSET(Ply[i].ply, PEAVES))
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

/**********************************************************************/
/*				broadcast_rom			      */
/**********************************************************************/

/* This function outputs a message to everyone in the room specified */
/* by the integer in the second parameter.  If the first parameter   */
/* is greater than -1, then if the player specified by that file     */
/* descriptor is present in the room, he is not given the message    */

void broadcast_rom(ignore, rm, fmt, i1, i2, i3, i4, i5, i6)
int	ignore, rm;
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n"); */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->rom_num == rm && Ply[i].ply->fd > -1
			   && i != ignore )
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

/**********************************************************************/
/*				broadcast_rom2			      */
/**********************************************************************/

/* This function is the same as broadcast_rom except that it will ignore */
/* two people in a room.						 */

void broadcast_rom2(ignore1, ignore2, rm, fmt, i1, i2, i3, i4, i5, i6)
int	ignore1, ignore2, rm;
char 	*fmt;
int	i1, i2, i3, i4, i5, i6;
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, fmt);
/*  strcat(fmt2, "\n"); */
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->rom_num == rm && Ply[i].ply->fd > -1
			   && i != ignore1 && i != ignore2)
				print(i, fmt2, i1, i2, i3, i4, i5, i6);
	}
}

/************************************************************************/
/*				inetname				*/
/************************************************************************/

/* This function returns the internet address of the address structure	*/
/* passed in as the first parameter.					*/

char *inetname(in)
struct in_addr in;
{
	register char *cp=0;
	static char line[50];
	struct hostent *hp;
	struct netent *np;
	static char domain[81];
	static int first = 1;
	int net, lna;

#ifdef GETHOSTBYNAME
	if (first) {
		first = 0;
		if (gethostname(domain, 80) == 0 &&
		    (cp = index(domain, '.')))
			(void) strcpy(domain, cp + 1);
		else
			domain[0] = 0;
	}
	cp = 0;
	if (in.s_addr != INADDR_ANY) {
		net = inet_netof(in);
		lna = inet_lnaof(in);

		if (lna == INADDR_ANY) {
			np = getnetbyaddr(net, AF_INET);
			if (np)
				cp = np->n_name;
		}
		if (cp == 0) {
			hp = gethostbyaddr((char *)&in, sizeof (in), AF_INET);
			if (hp) {
				if ((cp = index(hp->h_name, '.')) &&
				    !strcmp(cp + 1, domain))
					*cp = 0;
				cp = hp->h_name;
			}
		}
	}
#endif
	if (in.s_addr == INADDR_ANY)
		strcpy(line, "*");
	else if (cp)
		strcpy(line, cp);
	else {
		in.s_addr = ntohl(in.s_addr);
#define C(x)	((x) & 0xff)
		sprintf(line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			C(in.s_addr >> 16), C(in.s_addr >> 8), C(in.s_addr));
	}
	return (line);
}

/************************************************************************/
/*				add_wait				*/
/************************************************************************/

/* This function adds the descriptor in the first parameter to the	*/
/* waiting queue.							*/

void add_wait(fd)
int	fd;
{
	wq_tag	*new_wq, *wq;

	new_wq = (wq_tag *)malloc(sizeof(wq_tag));
	new_wq->next_tag = 0;
	new_wq->fd = fd;

	if(!First_wait) {
		First_wait = new_wq;
		Numwaiting = 1;
	}
	else {
		wq = First_wait;
		while(wq->next_tag)
			wq = wq->next_tag;
		wq->next_tag = new_wq;
		Numwaiting++;
	}
	print(fd, "The game is full.\nYou are player #%d in the waiting queue.\n", Numwaiting);
}

/************************************************************************/
/*				remove_wait				*/
/************************************************************************/

/* This function removes the i'th player from the waiting queue.	*/

int remove_wait(i)
int	i;
{
	int	j, fd;
	wq_tag	*wq, *prev;
	long	t;
	char 	str[50];

	wq = First_wait;

/* write is being used here to check to see if the given
  file desc. is still valid.  Theorically, only one write is
  needed, but for some reason 2 writes are needed */

 write(wq->fd," ",1);
        if(i == 1) {
                if (write(wq->fd,"\n",1) == -1){
                fd = -1;
		}
                else
                fd = wq->fd;
                First_wait = wq->next_tag;
        }
	else {
		for(j=1; j<i; j++) {
			prev = wq;
			wq = wq->next_tag;
		}
		fd = wq->fd;
		prev->next_tag = wq->next_tag;
	}

	free(wq);
	Numwaiting--;

	for(wq=First_wait, j=1; wq; wq=wq->next_tag, j++)
		if(j >= i) print(wq->fd, "You are player #%d in the waiting queue.\n", j);

	output_buf();
	return(fd);
}

void waiting(fd, param, str)
int	fd;
int	param;
char	*str;
{
	RETURN(fd, waiting, 1);
}

/************************************************************************/
/*				child_died				*/
/************************************************************************/

/* This function gets called when a SIGCHLD signal is sent to the	*/
/* program.								*/

void child_died()
{
	Deadchildren++;
	signal(SIGCHLD, child_died);
}

/************************************************************************/
/*				reap_children				*/
/************************************************************************/

/* This program goes through and kills off (waits for) each child	*/
/* that has completed processing to avoid zombie processes.		*/
/* If the child was an authentication process, then the information	*/
/* it returned is looked up in the file it saved.			*/

void reap_children()
{
	int pid, i, found, status;
	char filename[127], userid[80], address[80], timestr[80];
	FILE *fp;
	long t;

	t = time(0);
	strcpy(timestr, (char *)ctime(&t));
	timestr[strlen(timestr)-1] = 0;

	while(Deadchildren > 0) {
		Deadchildren--;
		found = -1;
		pid = wait(&status);
		sprintf(filename, "%s/auth/lookup.%d", LOGPATH, pid);
		for(i=0; i<Tablesize; i++) {
			if(Ply[i].io && Ply[i].io->lookup_pid == pid) {
				found = i;
				break;
			}
		}
		if(found < 0) {
			if(file_exists(filename)) unlink(filename);
			continue;
		}
		fp = fopen(filename, "r");
		if(!fp) continue;
		fscanf(fp, "%s %s", userid, address);
		if(strlen(userid))
			userid[8] = 0;
		if(strlen(address))
			address[39] = 0;
		fclose(fp);
		unlink(filename);
		strcpy(Ply[found].io->userid, userid);
		logf("%s: %s@%s (%s@%s) 접속\n", timestr,
			userid, address, userid, Ply[found].io->address);
		if(strcmp(address, "UNKNOWN"))
			strcpy(Ply[found].io->address, address);
	}

	/* just in case, kill off any zombies */
	wait4(-1, &status, WNOHANG, (struct rusage *)0);

}
