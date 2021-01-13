/*
 * AUTH.C:
 *
 *	Performs user authentication services.  Retrieves the
 *	host name of the user.  Also retrieves the user id if
 *	the host is running identd.
 *
 *	Copyright (C) 1994   Brett J. Vickers
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/signal.h>
#define MIGNORE
#include "mstruct.h"
#include "mextern.h"
#include "strstr.c"

char Output[2][80];
char *start_auth();
void parse();
int abort();

main(argc, argv)
int argc;
char *argv[];
{
	FILE *fp;
	char ipaddr[80], filename[127];
	int oport, iport;
	char buf[80];
	struct hostent *h;
	struct in_addr in;
	struct itimerval timer_value;

	timer_value.it_interval.tv_sec = 0L;
	timer_value.it_interval.tv_usec = 0L;
	timer_value.it_value.tv_sec = 20L;
	timer_value.it_value.tv_usec = 0L;

	setitimer(ITIMER_REAL, &timer_value, 0);

	signal(SIGALRM, abort);

	if(argc != 4) {
		fprintf(stderr, "Syntax: %s ip_address oport iport\n", argv[0]);
		exit(0);
	}

	strcpy(ipaddr, argv[1]);
	oport = atoi(argv[2]);
	iport = atoi(argv[3]);
	in.s_addr = inet_addr(ipaddr);

	/* Perform the host name lookup on the IP address*/
	h = gethostbyaddr((char *)&in, sizeof(in), AF_INET);
	if(!h)
		strcpy(Output[1], "UNKNOWN");
	else
		strcpy(Output[1], h->h_name);

	/* Begin the user id lookup */
	if(start_auth(&in, oport, iport, buf))
		strcpy(Output[0], buf);
	else
		strcpy(Output[0], "no_port");

	/* Save the results to a file so frp can look them up */
	sprintf(filename, "%s/auth/lookup.%d", LOGPATH, getpid());
	fp = fopen(filename, "w");
	if(!fp) exit(-1);
	fprintf(fp, "%s\n%s\n", Output[0], Output[1]);
	fclose(fp);

	exit(0);
}

/************************************************************************/
/*				start_auth				*/
/************************************************************************/

/* This function opens a socket to the identd port of the player's	*/
/* machine (port 113) and requests the user id of that player.		*/

char *start_auth(in, oport, iport, buf)
struct in_addr *in;
int oport, iport;
char *buf;
{
	struct sockaddr_in sin;
	int fd, i, n, Tablesize, len;
	fd_set Sockets, sockcheck;
	struct timeval t;
	char outstr[80];

	t.tv_sec = 10L;
	t.tv_usec = 0L;

	sin.sin_addr.s_addr = in->s_addr;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(113);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) return((char *)0);

	if(connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		return((char *)0);

	FD_ZERO(&Sockets);
	FD_SET(fd, &Sockets);

	/* Generate request string */
	sprintf(outstr, "%d , %d\r\n", oport, iport);
	len = strlen(outstr);

	Tablesize = getdtablesize();
	sockcheck = Sockets;

	/* Wait until we can send our request */
	if(select(Tablesize, 0, &sockcheck, 0, &t) <= 0)
		return((char *)0);

	/* Send the request */
	n = write(fd, outstr, len);
	if(n < len) return((char *)0);

	/* Wait until the other side responds */
	sockcheck = Sockets;
	if(select(Tablesize, &sockcheck, 0, 0, &t) <= 0)
		return((char *)0);

	/* Read the response */
	n = read(fd, buf, 80);
	close(fd);
	if(n <= 0)
		return((char *)0);
	else {
		buf[n-1] = 0;
		parse(buf);
		return(buf);
	}
}

/************************************************************************/
/*				parse					*/
/************************************************************************/

/* This function parses the response from the identity server.  It	*/
/* essentially strips out the user id if there is one to be obtained.	*/

void parse(buf)
char *buf;
{
	char *p, temp[80];

	if(p = (char *)strstr(buf, "USERID")) {
		while(*p && *p != ':') p++; p++;
		while(*p && *p != ':') p++;
		while(*p &&
		      !(*p <= '9' && *p >= '0') &&
		      !(*p <= 'z' && *p >= 'a') &&
		      !(*p <= 'Z' && *p >= 'A')) p++;
		strcpy(temp, p);
		p = temp;
		while(*p &&
			((*p >= 'a' && *p <='z') ||
			 (*p >= 'A' && *p <='Z') ||
		         (*p >= '0' && *p <= '9'))) p++;
		*p = 0;
		strcpy(buf, temp);
		buf[8] = 0;
	}
	else {
		strcpy(buf, "unknown");
	}

}

/************************************************************************/
/*				logf					*/
/************************************************************************/

/* Log debugging information if necessary.				*/

void logf(fmt, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)
char    *fmt;
int     i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
{
        char    file[80];
        char    str[1024];
        int     fd;

        sprintf(file, "%s/authlog", LOGPATH);
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

int abort()
{
	exit(0);
}
