/* ITALK.C:Internet Talking:  Copyright 1991, 1992 Steven D. Wallace */
/*   Public domain.  For non-commercial use only.
/*                                                                         */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <sys/file.h>
#include <ctype.h>
#include <signal.h>
#define TIOCGETP        _IOR(0, 8,struct sgttyb)/* get parameters -- gtty */
#define TIOCSETP        _IOW(0, 9,struct sgttyb)/* set parameters -- stty */ 
/*
 * Structure for TIOCGETP and TIOCSETP ioctls.
 */
 
#ifndef _SGTTYB_
#define _SGTTYB_
struct  sgttyb {
        char    sg_ispeed;              /* input speed */
        char    sg_ospeed;              /* output speed */
        char    sg_erase;               /* erase character */
        char    sg_kill;                /* kill character */
        short   sg_flags;               /* mode flags */
};
#endif
#if 1 /* 0 for computers that don't have h_addr_list in struct hostent */
#define h_addr  h_addr_list[0] /* for backwards compatibilty */
#endif

#define HOSTFILE ".ithost"
#define FILE1 "/home/mordor/bin/.lphost"
#define SDWFILE FILE1

#define MAXMACRO 100
#define MAXDIGIT 2

/* TERM.C */

extern void     new_term(), old_term();
extern char     get_key(), in_key();

extern errno;
int sock;
int df_i = 1;
int df_o = 2;
char quiet = 0;

void aborting();
void get_address();
void split_string();

main(argc, argv)
int argc;
char *argv[];
{
  char  address[30], port[10], path[80], startup[20], macro[20], blank[80];
  char  *address_ptr, *port_ptr, *startup_ptr, *macro_ptr;
  int   i = 0;
  char	command[MAXMACRO][80];

  address[0] = port[0] = path[0] = startup[0] = macro[0] = blank[0] = 0;
  signal(SIGPIPE,SIG_IGN);

  i = 1;
  if(i < argc) {
    if(argv[i][0] == '-' && argv[i][1] == 'q') {
      quiet = 1;
      i++;
    }
  }
  if(i < argc) {
    strcpy(address, argv[i++]);
    if(i < argc)
      strcpy(port, argv[i++]);
  }
  else
    get_address(address, port);

  /* determine user's pathname to their .ithost file */
  sprintf(path, "%s/%s", getenv("HOME"), HOSTFILE);

  /* if no dot in address string argument, good chance it is a symbol */
  if (!dot_in_string(address)) {

    address_ptr = address;

    /* if no valid port given as argument, read in new startup */
    if (!*port)
      port_ptr = port;
    else
      port_ptr = blank;

    /* then look up address in .ithost in user's home directory */
    if (!read_address(path, address, address_ptr, port_ptr, startup, macro))

      /* not in their home directory, so try ~swallace/.lphost */
      read_address(SDWFILE, address, address_ptr, port_ptr, startup, macro);
    else

      /* lookup good, see if address is a reference to global symbol */
      if (*address == '!') {

	if (*port != '!')
	  port_ptr = blank;

	/* if no valid startup given, read in global startup */
	if (!*startup || *startup == '#')
	  startup_ptr = startup;
	else
	  startup_ptr = blank;

	/* if no valid macro given, read in global startup */
	if (!*macro || *macro == '#')
	  macro_ptr = macro;
	else
	  macro_ptr = blank;

        read_address(SDWFILE, address + 1, address_ptr, port_ptr, startup_ptr, 
macro_ptr);
      }

    }

  if (*port == '#') 	*port = 0;
  if (*startup == '#')	*startup = 0;
  if (*macro == '#')	*macro = 0;

/*  printf("addr: %s, port: %s, startup: %s, macro: %s\n", address, port, start
up, macro); */

  sock = connect_sock(address, port);
  if (sock == -1)
    aborting(3);

  /* clear macros */
  for(i=0; i != MAXMACRO; i++)
    command[i][0] = 0;

  /* determine user's pathname to their .macro or whatever file */
  sprintf(path, "%s/%s", getenv("HOME"), macro);

  /* read macro from user's file */
  if(*macro)
   if (read_macro(path, command) && !quiet)
    fprintf(stderr, "Macros read from file %s.\n", path);

  if (*startup) {
    sprintf(path, "%s/%s", getenv("HOME"), startup);
    write_file(path);
  }

  if (socket_io_handler(sock, command) == -1)
    aborting(4);

}


int connect_sock(address, port)
char *address;
char *port;
{
  int  temp_int;
  char name[80];
  char number[20];
  unsigned long addr;
  unsigned long saddr;
  struct sockaddr_in sin;
  struct hostent *host;

  if(isdigit(address[0])) {
    if((addr = inet_addr(address)) == -1)
      aborting(5);
    else
      saddr = ntohl(addr);
      host = gethostbyaddr((char *)&saddr, sizeof(saddr), AF_INET);
      if(host)
        strncpy(name, host->h_name, 79);
      else
        strcpy(name, address);

      strcpy(number, address);
  }
  else {
    host = gethostbyname(address);
    if(!host) aborting(6);
    addr = *(long *)(host->h_addr);
    strcpy(name, host->h_name);

    saddr = ntohl(addr);
    sprintf(number, "%d.%d.%d.%d",
        (unsigned)(saddr)>>24,
        (saddr & 0x00ff0000)>>16,
        (saddr & 0x0000ff00)>>8,
        (saddr & 0x000000ff));
  }
  
  sin.sin_addr.s_addr = addr;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(atoi(port) ? atoi(port) : 23);

  if(!quiet) {
    if(*port)
      fprintf(stderr, "Trying %s, port %s ...\n", number, port);
    else
      fprintf(stderr, "Trying %s ...\n", number);
  }

  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket");
    return(-1);
  }
  if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("Connect");
    return(-1);
  }

  temp_int = 1;
  ioctl(sock, FIONBIO, &temp_int);

  if(!quiet) {
    if(*port)
      fprintf(stderr, "Connected to %s, port %s.\n", name, port);
    else
      fprintf(stderr, "Connected to %s.\n", name);
  }

  return(sock);
}

int socket_io_handler(sock, command)
int sock;
char *command;
{
  int  readfds;
  int  len, i;
  char echo = 1;
  char buf[1024];
  char *temp, tc;
  struct sgttyb arg;

  while(1) {
    readfds = 1<<df_i | 1<<sock;

    while(!select(16, &readfds, 0, 0, 0)) ;
    if(readfds & (1<<df_i)) {
      len = read(df_i, buf, 1023);
      if(len == 0) aborting(1);
      if( ( (buf[0] >= '0') && (buf[0] <= '9') ) &&
      	 !( (buf[MAXDIGIT] >= '0') && (buf[MAXDIGIT] <= '9') ) ) {
	tc = buf[MAXDIGIT];
	buf[MAXDIGIT] = 0;
	i = atoi(buf);
        temp = command + i*80;
	if(*temp && (i < MAXMACRO)) {
          if(write(sock, temp, (len = strlen(temp))) < 0) {
            perror("write sock");
            return(0);
          }
	}
	else {
	  buf[MAXDIGIT] = tc;
          if(write(sock, buf, len) < 0) {
            perror("write sock");
            return(0);
          }
	}
      }
      else
        if(write(sock, buf, len) < 0) {
          perror("write sock");
          return(0);
        }
    }

    if(readfds & (1<<sock)) {
      len = read(sock, buf, 1023);
      if(len == 0) aborting(2);

      if (buf[0] == -1) {

	if (buf[1] == -5 && buf[2] == 1) {
	  ioctl(0,TIOCGETP,&arg);	/* Get initial setup */
	  arg.sg_flags &= ~(ECHO);	/* Turn off terminal ECHO */
	  ioctl(0,TIOCSETP,&arg);	/* Store new settings */
	  echo = 0;
      	    if (len > 3) if(write(df_o, buf+3, len-3) < 0) {
              perror("write terminal");
              return(0);
			 }
	}

	else if (buf[1] == -4 && buf[2] == 1) {
	  ioctl(0,TIOCGETP,&arg);	/* Get initial setup */
	  arg.sg_flags |= (ECHO);	/* Turn on terminal ECHO */
	  ioctl(0,TIOCSETP,&arg);	/* Store new settings */
	  echo = 1;
      	    if (len > 3) if(write(df_o, buf+3, len-3) < 0) {
              perror("write terminal");
              return(0);
			 }
	}

        else if(write(df_o, buf, len) < 0) {
          perror("write terminal");
          return(0);
        }

      } /* end FF special check */

      else if(write(df_o, buf, len) < 0) {  /* no FF, so output string */
        perror("write terminal");
        return(0);
      }

      if(!echo) {
	for(i = 0; i < len-2; i++)
	  if(buf[i] == -1)
            if(buf[i] == -1 && buf[i+1] == -4 && buf[i+2] == 1) {
	      ioctl(0,TIOCGETP,&arg);	/* Get initial setup */
	      arg.sg_flags |= (ECHO);	/* Turn on terminal ECHO */
	      ioctl(0,TIOCSETP,&arg);	/* Store new settings */
              echo = 1;
	    }
      } /* end no echo to echo check */

    } /* end socket out check */

  }  /* end infinite loop */
} /* end io_handler */

void get_address(address, port)
char *address;
char *port;
{
  char buf[51];

  write_msg("Destination: ");
  read_msg(buf,50);

  split_string(buf, address, port);
}

void split_string(str_in, str_one, str_two)
char *str_in;
char *str_one;
char *str_two;
{
  int i=0, j=0;

  while(str_in[i] != ' ' && str_in[i] != '\n' && str_in[i] != 0)
    str_one[j++] = str_in[i++];
  str_one[j]=0; j=0;
  while(str_in[i] == ' ' || str_in[i] == '\n') i++;
  if(str_in[i])
    while(str_in[i] != 0 && str_in[i] != '\n')
      str_two[j++] = str_in[i++];
  str_two[j]=0;
}

int dot_in_string(str)
char *str;
{
  int i=0;

  while (str[i++] != 0)
    if (str[i-1] == '.')
      return(1);

  return(0);
}

int read_macro(fname, command)
char  *fname;
char  *command;
{
  FILE *fp;
  char buf[81];
  char numbuf[21];
  char *temp;
  int  num, i;

  if((fp=fopen(fname, "r")) == 0)
    return(0);

  while(!feof(fp)) {
    fgets(buf, sizeof(buf), fp);
    if(*buf == '#')
      continue;
    split_string(buf, numbuf, buf);

    num = atoi(numbuf);
    temp = command + num*80;
    if(num) {
      strcpy(temp, buf);
      num = strlen(temp);

      /* replace all '\' with CR/LF? */
/*
      for (i = 0; i != num; i++) {
	if (temp[i] == '\\')
	  temp[i] = 13;
      }
*/

      /* add CR to end of string */
      if (temp[num-1] != 10) {
	temp[num++] = 13;
	temp[num]=10;
	num = num +1;
      }
      temp[num] = 0;
    }
  }
  fclose(fp);
  return(1);
}

int read_address(fname, symbol, address, port, startup, macro)
char  *fname;
char  *symbol;
char  *address;
char  *port;
char  *startup;
char  *macro;
{
  FILE *fp;
  char buf[81];
  char cur_symbol[21];
  int  i;

  if((fp=fopen(fname, "r")) == 0)
    return(0);
  while(!feof(fp)) {
    fgets(buf, sizeof(buf), fp);/* get rest of line (address, port) */
    split_string(buf, cur_symbol, buf);

    /* check to see if parameter matches current symbol in line */
    for (i=0; symbol[i] != 0; i++)
      if (symbol[i] != cur_symbol[i])
        break;
    if (symbol[i] == cur_symbol[i]) {
      fclose(fp);
      split_string(buf, address, buf);
      if(*buf) split_string(buf, port, buf);
      if(*buf) split_string(buf, startup, buf);
      if(*buf) split_string(buf, macro, buf);
      return(1);
    } 
  }
  fclose(fp);
  return(0);
}

int write_file(path)
char  *path;
{
  int  fd;
  long count;
  char buf[1024];

  fd = open(path, O_RDONLY, 0);
  if(fd < 0) return(0);

  if(!quiet)
    fprintf(stderr, "Sending file %s to remote host.\n", path);

  while(1) {
    count = read(fd, buf, sizeof(buf));
    if(count)
      write(sock, buf, count);
    else
      break;
  }
  return(1);
}

write_msg(str)
char *str;
{
  write(df_o, str, strlen(str));
}

write_sock(str)
char *str;
{
  write(sock, str, strlen(str));
}

read_msg(str,slen)
char *str;
int slen;
{
  int readfds;
  int num=0, len;
  char ch;

  do {
    readfds = 1<<df_i;
    while(!select(16, &readfds, 0, 0, 0)) ;
    len = read(df_i, &ch, 1);
    if(len == 0) aborting(0);
    if(ch == 10 || num==slen-1) ch = 0;
    if(ch != 13) str[num++] = ch;
  } while(ch);

}

void aborting(arg)
int arg;
{
  switch (arg) {
	case 1:
		fprintf(stderr, "\nConnection closed by local host.\n");
		break;
	case 2:
		fprintf(stderr, "\nConnection closed by remote host.\n");
		break;
	case 3:
		fprintf(stderr, "\nUnable to connect to remote host.\n");
		break;
	case 4:
		fprintf(stderr, "\nProblems with sockets.  Connection closed.\n");
		break;
	case 5:
		fprintf(stderr, "\nInvalid host number.\n");
		break;
	case 6:
		fprintf(stderr, "\nInvalid host name.\n");
		break;
	default:
  		fprintf(stderr, "Connection closed (%d).\n",arg);
  }
  signal(SIGPIPE,SIG_IGN);
  close(sock); close(df_i); close(df_o);
  exit(0);
}


