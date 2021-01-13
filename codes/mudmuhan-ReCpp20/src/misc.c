/*
 * MISC.C:
 *
 *	Miscellaneous string, file and data structure manipulation
 *	routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>

/************************************************************************/
/*				merror					*/
/************************************************************************/

/* merror is called whenever an error message should be output to the	*/
/* log file.  If the error is fatal, then the program is aborted	*/

void merror(str, errtype)
char 	*str;
char 	errtype;
{
	long t;
	char bugstr[256];

	t = time(0);
	sprintf(bugstr, "Error occured in %s. %s", str, ctime(&t));
	logf(bugstr);
	if(errtype == FATAL)
		exit(-1);
}

/************************************************************************/
/*				lowercize				*/
/************************************************************************/

/* This function takes the string passed in as the first parameter and	*/
/* converts it to lowercase.  If the flag in the second parameter has	*/
/* its first bit set, then the first letter is capitalized.		*/

void lowercize(str, flag)
char	*str;
int	flag;
{
	int 	i, n;

	n = (str) ? strlen(str) : 0;
	for(i=0; i<n; i++)
		str[i] = (str[i] >= 'A' && str[i] <= 'Z') ? str[i]+32:str[i];
	if(flag & 1)
		str[0] = (str[0] >= 'a' && str[0] <= 'z') ? str[0]-32:str[0];
}

/************************************************************************/
/*				low					*/
/************************************************************************/

/* If the character passed in as the first parameter is an uppercase 	*/
/* alphabetic character, then it is converted to lowercase and returned */
/* Otherwise, it is unchanged.						*/

int low(ch)
char	ch;
{
	if(ch >= 'A' && ch <= 'Z')
		return(ch+32);
	else
		return(ch);
}

int up(ch)
char	ch;
{
	if(ch >= 'a' && ch <= 'z')
		return(ch-32);
	else
		return(ch);
}

/************************************************************************/
/*				zero					*/
/************************************************************************/

/* This function zeroes a block of bytes at the given pointer and the */
/* given length.						      */

void zero(ptr, size)
void 	*ptr;
int	size;
{
	char 	*chptr;
	int	i;

        chptr = (char *)ptr;
	for(i=0; i<size; i++) 
		chptr[i] = 0;
}
	
/* Temporary (but static) data for the next several functions */

static char	xstr[5][80];
static int	xnum=0;

/************************************************************************/
/*				crt_str					*/
/************************************************************************/

/* This function takes the creature given it in the first parameter, 	*/
/* and forms the appropriate singularized or pluralized version of the  */
/* creature's name using certain flags.					*/

char *crt_str(crt, num, flag)
creature	*crt;
int		num, flag;
{
	char	ch;
	char	*str;

	str = xstr[xnum];  xnum = (xnum + 1)%5;

	if(crt->type != MONSTER) {
		if(((F_ISSET(crt, PINVIS) || F_ISSET(crt, PDMINV)) && 
		   !(flag & 2)) || F_ISSET(crt, PDMINV) && !(flag&DMF))
			strcpy(str,"누군가");
		else {
			strcpy(str, crt->name);
			if(F_ISSET(crt, PINVIS))
				strcat(str, "(*)");
			if(!(flag&CAP)) strcat(str,"님");
		}
		return(str);
	}

	if(num == 0 || num == 1) {
			strcpy(str, crt->name);
	}
	else {
		sprintf(str, "(x%d) ", num);

		strcat(str, crt->name);
	/*	if(F_ISSET(crt, MTOMEN)) {
			str[strlen(str)-2] = 'e';
		}
		else if(!F_ISSET(crt, MDROPS)) {
			str[strlen(str)+1] = 0;
			str[strlen(str)+2] = 0;
			if(str[strlen(str)-1] == 's' || 
			   str[strlen(str)-1] == 'x') {
				str[strlen(str)] = 'e';
				str[strlen(str)] = 's';
			}
			else
				str[strlen(str)] = 's';
		}
	*/
	}

	/*if(flag & CAP)
		str[0] = up(str[0]);
	*/
	if((flag & MAG) && (crt->type != PLAYER) && (F_ISSET(crt, MMAGIC)))
		strcat(str, "(주문)");

	return(str);

}

/************************************************************************/
/*				obj_str					*/
/************************************************************************/

/* This function examines an object's flags and its name, and returns	*/
/* the appropriate pluralized or singularized version of the name.	*/
/* In some cases it is necessary to drop the s on a plural version of	*/
/* a word.								*/

char *obj_str(obj, num, flag)
object	*obj;
int	num, flag;
{
	char 	ch;
	char	str2[10];
	char	*str;
	int	i;

	str = xstr[xnum];  xnum = (xnum + 1)%5;

	/* 아이템 이름 뒤에 공백 없앰... */
	for(i=strlen(obj->name)-1;i>=0;i--) {
		if(obj->name[i]!=' ') break;
	}
	obj->name[i+1]=0;

	if(num == 0) {
		strcpy(str, obj->name);
		/*
		if(!F_ISSET(obj, ONOPRE)) {
			strcpy(str, "the ");
			strcat(str, obj->name);
		}
		else
			strcpy(str, obj->name);
		*/
	}
		
	else if(num == 1) {
	/*	if(F_ISSET(obj, ONOPRE) || obj->type == MONEY)
			strcpy(str, "");
		else if(F_ISSET(obj, OSOMEA))
			strcpy(str, "약간의 ");
		else {
			ch = low(obj->name[0]);
			if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' ||
			   ch == 'u')
				strcpy(str, "an ");
			else
				strcpy(str, "a ");
		}
	*/	strcpy(str, obj->name);
	}

	else {
		/*
	      if(num < 21)
			sprintf(str, "%s ", number[num]);
		else
			sprintf(str, "%d ", num);
		*/
		sprintf(str, "(x%d) ",num);


		strcat(str, obj->name);
		/* 한글에서는 복수 불필요. 
		if(!F_ISSET(obj, ODROPS) && !F_ISSET(obj, OSOMEA)) {
			str[strlen(str)+1] = 0;
			str[strlen(str)+2] = 0;
			if(str[strlen(str)-1] == 's' || 
			   str[strlen(str)-1] == 'x') {
				str[strlen(str)] = 'e';
				str[strlen(str)] = 's';
			}
			else
				str[strlen(str)] = 's';
		}
		*/
	}

	if(flag & CAP)
		str[0] = up(str[0]);
	if((flag & MAG) && obj->adjustment) {
		sprintf(str2, "(%s%d)", obj->adjustment >= 0 ? "+":"",
			obj->adjustment);
		strcat(str, str2);
	}
	else if((flag & MAG) && obj->magicpower)
		strcat(str, "(주문)");

	return(str);
}

/************************************************************************/
/*				delimit					*/
/************************************************************************/

/* This function takes a given string, and if it is greater than a given*/
/* number of characters, then it is split up into several lines.  This  */
/* is done by replacing spaces with carriage returns before the end of  */
/* the line.								*/

#define MAXLINE	77

void delimit(str)
char	*str;
{
	int 	i, j, l, len, lastspace;
	char 	str2[2048];

	str2[0] = 0;
	j = (str) ? strlen(str) : 0;
	if(j < MAXLINE)
		return;

	len = 0; lastspace = -1; l = 0;
	for(i=0; i<j; i++) {
		if(str[i] == ' ')
			lastspace = i;
		if(str[i] == '\n') {
			len = 0;
			lastspace = -1;
		}
		len++;
		if(len > MAXLINE && lastspace > -1) {
			str[lastspace] = 0;
			strcat(str2, &str[l]);
			strcat(str2, "\n  ");
			l = lastspace + 1;
			len = i - lastspace + 3;
			lastspace = -1;
		}
	}
	strcat(str2, &str[l]);
	strcpy(str, str2);
}

/************************************************************************/
/*				view_file				*/
/************************************************************************/

/* This function views a file whose name is given by the third 		*/
/* parameter. If the file is longer than 20 lines, then the user is 	*/
/* prompted to hit return to continue, thus dividing the output into 	*/
/* several pages.							*/

#define FBUF	800

void view_file(fd, param, str)
int fd, param;
char *str;
{
	char	buf[FBUF+1];
	int	i, l, n, ff, line;
	long	offset;

	buf[FBUF] = 0;
	switch(param) {
	case 1:
		offset = 0L;
		strcpy(Ply[fd].extr->tempstr[1], str);
		ff = open(str, O_RDONLY, 0);
		if(ff < 0) {
			print(fd, "화일을 읽을 수 없습니다.\n");
			RETURN(fd, command, 1);
		}
		line = 0;
		while(1) {
			n = read(ff, buf, FBUF);
			l = 0;
			for(i=0; i<n; i++) {
				if(buf[i] == '\n') {
					buf[i] = 0;
					line++;
					print(fd, "%s\n", &buf[l]);
					offset += (i-l+1);
					l = i+1;
				}
				if(line > 18) break;
			}
			if(line > 18) {
				sprintf(Ply[fd].extr->tempstr[0], "%lu", 
					offset);
				break;
			}
			else if(l != n) {
				print(fd, "%s", &buf[l]);
				offset += (i-l);
			}
			if(n<FBUF) break;
		}
		if(n==FBUF || line>18) {
			F_SET(Ply[fd].ply, PREADI);
			print(fd, "\n[엔터]를 누르세요. 그만보시려면 [.]을 치세요: ");
			output_buf();
			Ply[fd].io->intrpt &= ~1;
		}
		if(n<FBUF && line <= 18) {
			close(ff);
                      F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		else {
			close(ff);
			F_SET(Ply[fd].ply, PREADI);
			RETURN(fd, view_file, 2);
		}
	case 2:
		if(str[0] == '.') {
			print(fd, "중단합니다.\n");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		offset = atol(Ply[fd].extr->tempstr[0]);
		ff = open(Ply[fd].extr->tempstr[1], O_RDONLY, 0);
		if(ff < 0) {
			print(fd, "화일이 없습니다. 신에게 연락해 주세요.\n");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		lseek(ff, offset, 0);
		line = 0;
		while(1) {
			n = read(ff, buf, FBUF);
			l = 0;
			for(i=0; i<n; i++) {
				if(buf[i] == '\n') {
					buf[i] = 0;
					line++;
					print(fd, "%s\n", &buf[l]);
					offset += (i-l+1);
					l = i+1;
				}
				if(line > 18) break;
			}
			if(line > 18) {
				sprintf(Ply[fd].extr->tempstr[0], "%lu", 
					offset);
				break;
			}
			else if(l != n) {
				print(fd, "%s", &buf[l]);
				offset += (i-l);
			}
			if(n<FBUF) break;
		}
		if(n==FBUF || line > 18) {
			F_SET(Ply[fd].ply, PREADI);
			print(fd, "\n[엔터]를 누르세요. 그만보시려면 [.]을 치세요: ");
			output_buf();
			Ply[fd].io->intrpt &= ~1;
		}
		if(n<FBUF && line <= 18) {
			close(ff);
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		else {
			close(ff);
			RETURN(fd, view_file, 2);
		}
	}
}

/************************************************************************/
/*				dice					*/
/************************************************************************/

/* This function rolls n s-sided dice and adds p to the total.		*/

int dice(n, s, p)
int 	n, s, p;
{
	int	i;

	for(i=0; i<n; i++)
		p += mrand(1,s);

	return(p);
}

/************************************************************************/
/*				exp_to_lev				*/
/************************************************************************/

/* This function takes a given amount of experience as its first 	*/
/* argument returns the level that the experience reflects.		*/ 

int exp_to_lev(exp)
long 	exp;
{
	int level = 1;
	
	while (exp >= needed_exp[level-1] && level < MAXALVL)
		level++;
	if (level >= MAXALVL) {
		level = (exp-needed_exp[MAXALVL-2])/5000000+MAXALVL;
	}

	return(MAX(1,level));
}

/************************************************************************/
/*				dec_daily				*/
/************************************************************************/

/* This function is called whenever a daily-use item or operation is	*/
/* used or performed.  If the number of daily uses are used, up then	*/
/* a 0 is returned.  Otherwise, the number of uses is decremented and	*/
/* a 1 is returned.							*/

int dec_daily(dly_ptr)
struct daily	*dly_ptr;
{
	long		t;
	struct tm	*tm, time1, time2;

	t = time(0);
	tm = localtime(&t);

	time1 = *tm;

	tm = localtime(&dly_ptr->ltime);

	time2 = *tm;

	if(time1.tm_yday != time2.tm_yday) {
		dly_ptr->cur = dly_ptr->max;
		dly_ptr->ltime = t;
	}

	if(dly_ptr->cur == 0)
		return(0);

	dly_ptr->cur--;

	return(1);
}

/************************************************************************/
/*				logf					*/
/************************************************************************/

/* This function writes a formatted printf string to a logfile called	*/
/* "log" in the player directory.					*/

void logf(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char 	*fmt;
int	i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
{
	char	file[80];
	char	str[1024];
	int	fd;

	sprintf(file, "%s/log", LOGPATH);
	fd = open(file, O_RDWR, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd < 0) return;
	}
	lseek(fd, 0L, 2);

	sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);

	write(fd, str, strlen(str));

	close(fd);

}

void log_dm(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char 	*fmt;
int  i1, i2, i3, i4, i5, i6, i7, i9, i10;
{
	char file[80];
	char str[1024];
	int fd;

	sprintf(file, "%s/log_dm", LOGPATH);
	fd = open(file, O_RDWR, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd < 0) return;
	}
	lseek(fd, 0L, 2);

	sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
	write(fd, str, strlen(str));
	close(fd);
}

void log_dmcmd(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char 	*fmt;
int  i1, i2, i3, i4, i5, i6, i7, i9, i10;
{
	char file[80];
	char str[1024],str2[1024];
	int fd;
    long t;

	sprintf(file, "%s/log_dmcmd", LOGPATH);
	fd = open(file, O_RDWR, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd < 0) return;
	}
	lseek(fd, 0L, 2);

	t=time(0);
	strcpy(str2,(char *)ctime(&t));
	str2[strlen(str2)-1] = 0;
	strcat(str2," : ");
	sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
	write(fd, str2, strlen(str2));
	write(fd, str, strlen(str));
	close(fd);
}

void log_fl(cur_time, cur_name)
int cur_time, cur_name;
{
	char file[80], file2[80];
	char str[1024], str2[1024];
	int fd, fd2;
	
	sprintf(file, "%s/log_fl", LOGPATH);
	fd = open(file, O_RDWR, 0);
	if(fd<0) {
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd<0) return;
	}
	sprintf(file2, "%s/fal/%s", PLAYERPATH, cur_name);
	fd2 = open(file2, O_RDWR, 0);
	if(fd2 < 0) {
		fd2 = open(file2, O_RDWR | O_CREAT, ACC);
		if(fd2 < 0) return;
	}

	lseek(fd, 0L, 2);
	lseek(fd2, 0L, 2);

	sprintf(str, "%s : %s 님의 아이디로 접속시도.\n", cur_time, cur_name);
	sprintf(str2, "( %s )에 접속시도가 있었습니다.\n", cur_time);
	write(fd, str, strlen(str));
	write(fd2, str2, strlen(str2));

	close(fd);
	close(fd2);
}

/************************************************************************/
/*				sort_cmds				*/
/************************************************************************/

/* This function sorts the global cmdlist structure array so that the	*/
/* command list is sorted by how often each command is used.  The most-	*/
/* used commands will end up at the top of the list.			*/

void sort_cmds()
{
	Cmdnum = 0;

	while(cmdlist[Cmdnum].cmdno != 0) Cmdnum++;

	qsort((void *)cmdlist, Cmdnum, sizeof(struct cmdstruct), sort_cmp);

}

/************************************************************************/
/*				sort_cmp				*/
/************************************************************************/

/* This function is used by the quicksort routine to sort the command	*/
/* list according to how often each command has been used.		*/

int sort_cmp(arg1, arg2)
struct cmdstruct *arg1, *arg2;
{
	return(strcmp(arg1->cmdstr, arg2->cmdstr));
}

/************************************************************************/
/*				file_exists				*/
/************************************************************************/

/* This function returns 1 if the filename specified by the first par-	*/
/* ameter exists, 0 if it doesn't.					*/

int file_exists(filename)
char *filename;
{
	int ff;

	ff = open(filename, O_RDONLY);
	if(ff > -1) {
		close(ff);
		return(1);
	}
	else
		return(0);
}

/************************************************************************/
/*				load_lockouts				*/
/************************************************************************/

/* This function opens the lockout file and reads in all locked out 	*/
/* sites.								*/

void load_lockouts()
{
	FILE	*fp;
	char	str[80];
	int	i;

	if(Lockout) free(Lockout);
	Numlockedout = 0;

	sprintf(str, "%s/lockout", LOGPATH);
	fp = fopen(str, "r");
	if(!fp) return;

	while(1) {
		if(fscanf(fp, "%s", str) == EOF) break;
		if(fscanf(fp, "%s", str) == EOF) break;
		Numlockedout++;
	}

	if(!Numlockedout) {
		fclose(fp);
		return;
	}

	Lockout = (lockout *)malloc(Numlockedout * sizeof(lockout));
	fseek(fp, 0L, 0);

	for(i=0; i<Numlockedout; i++) {
		fscanf(fp, "%s", Lockout[i].address);
		fscanf(fp, "%s", Lockout[i].password);
		if(Lockout[i].password[0] == '-' && !Lockout[i].password[1])
			Lockout[i].password[0] = 0;
	}

	fclose(fp);
}

/************************************************************************/
/*				please_wait				*/
/************************************************************************/

void please_wait(fd, duration)
int	fd;
int	duration;
{
	if(duration == 1)
		print(fd, "1초만 기다리세요.\n");
	else
		print(fd, "%d초동안 기다리세요.\n", duration);
}
 
/************************************************************************/
/*                              logn                                   */
/************************************************************************/
 
/* This function writes a formatted printf string to a logfile called   */
/* "log" in the player directory.                                       */
 
void logn(name,fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char    *name;
char    *fmt;
int     i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
{
	char    file[80];
        char    str[1024];
        int     fd;
 
        sprintf(file, "%s/%s", LOGPATH,name);
        fd = open(file, O_RDWR, 0);
        if(fd < 0) {
                fd = open(file, O_RDWR | O_CREAT, ACC);
                if(fd < 0) return;
        }
        lseek(fd, 0L, 2);
 
        sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
 
        write(fd, str, strlen(str));
 
        close(fd);
}

void log_overwrite(name,fmt,i1,i2,i3,i4,i5,i6)
char *name;
char *fmt;
int i1,i2,i3,i4,i5,i6;
{
    char file[80];
    char str[1024];
    int fd;

    sprintf(file,"%s/%s",LOGPATH,name);
    fd = open(file, O_RDWR|O_CREAT, ACC);
    if(fd<0) return;

    lseek(fd,0L,2);

    sprintf(str,fmt,i1,i2,i3,i4,i5,i6);
    write(fd,str,strlen(str));
    close(fd);
}

/*====================================================================*/
int isnumber(str)
char    *str;
/* checks if the given str contains all digits */

{
  int len, i;
   len = strlen(str);
   for (i=0;i < len; i++)
      if(!isdigit(str[i]))
         return (0);
   return (1);
}
/*====================================================================*/

void log_pl(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char    *fmt;
int  i1, i2, i3, i4, i5, i6, i7, i9, i10;
{
    char file[80];
    char str[1024];
    int fd;

    sprintf(file, "%s/log_pl", LOGPATH);
    fd = open(file, O_RDWR, 0);
    if(fd < 0) {
        fd = open(file, O_RDWR | O_CREAT, ACC);
        if(fd < 0) return;
    }
    lseek(fd, 0L, 2);

    sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    write(fd, str, strlen(str));
    close(fd);
}

void log_plcmd(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char    *fmt;
int  i1, i2, i3, i4, i5, i6, i7, i9, i10;
{
    char file[80];
    char str[1024];
    int fd;

    sprintf(file, "%s/log_plcmd", LOGPATH);
    fd = open(file, O_RDWR | O_CREAT, ACC);
    if(fd < 0) {
        if(fd < 0) return;
    }
    lseek(fd, 0L, 2);

    sprintf(str, fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    write(fd, str, strlen(str));
    close(fd);
}
