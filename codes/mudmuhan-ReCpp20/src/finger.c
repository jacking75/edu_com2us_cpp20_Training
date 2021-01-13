/*
 * FINGER.C:
 *
 *	Remote finger functions
 *
 *	Copyright (C) 1992, 1993 Brett J. Vickers
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h> 

#define write_fd(a,b)	write(a, b, strlen(b));

main(argc, argv) 
int	argc;
char	*argv[];
{
	void finger();
	int fd;

	if(argc < 3) exit(0);

	fd = atoi(argv[1]);
	if(argc == 4)
		finger(fd, argv[2], argv[3]);
	else
		finger(fd, argv[2], "");
}

/************************************************************************/
/*				finger					*/
/************************************************************************/

void finger(fd, address, person)
int	fd;
char	*address;
char	*person;
{
	int			new_sock, n, Tablesize;
	char			buf[80];
	unsigned long		addr;
	struct sockaddr_in	sin;
	struct hostent		*h;
	fd_set			mask;

	if((addr = inet_addr(address)) != -1)
		sin.sin_family = AF_INET;

	else if((h = gethostbyname(address)) != NULL) {
		sin.sin_family = h->h_addrtype;
		bcopy(h->h_addr, &addr, h->h_length);
	}

	else {
		write_fd(fd, "\n\rInvalid host address\n\r");
		exit(0);
	}

	sin.sin_addr.s_addr = addr;
	sin.sin_port = htons(79);

	if((new_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		write_fd(fd, "\n\rUnable to connect to finger server.\n\r");
		exit(0);
	}

	if(connect(new_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		write_fd(fd, "\n\rUnable to connect to finger server.\n\r");
		exit(0);
	}

	write(new_sock, person, strlen(person));
	write(new_sock, "\n\r\n\r", 4);

	FD_ZERO(&mask);

	Tablesize = getdtablesize();

	while(1) {
		FD_SET(new_sock, &mask);
		select(Tablesize, &mask, 0, 0, 0);
		n = read(new_sock, buf, 80);
		if(n <= 0) {
			close(new_sock);
			exit(0);
		}
		write(fd, buf, n);
	}
}
