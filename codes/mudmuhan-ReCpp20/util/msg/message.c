#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>

#define saddr ntohl(sin.sin_addr.s_addr)
#define PORT 4040
#define FILENAME "/home/mordor/.msg"

extern errno;
int cfd, s2;
void aborting();

main(argc, argv)
int argc;
char *argv[];
{
  int s,i;
  int fd;
  int len = sizeof(struct sockaddr_in);
  struct servent *ps;
  struct sockaddr_in sin;

  signal(SIGPIPE,SIG_IGN);

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    exit(1);
  }

  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(PORT);

  i = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)); 

  if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    exit(1);
  }

  listen(s, 5);
  while(1) {
    len = sizeof(struct sockaddr_in);
    if((fd = accept(s, (struct sockaddr *) &sin, &len)) < 0) {
      exit(1);
    }

    log("connection accepted from: %d.%d.%d.%d\n", 
	(unsigned)(saddr)>>24,
	(saddr & 0x00ff0000)>>16,
	(saddr & 0x0000ff00)>>8,
	(saddr & 0x000000ff));

    if(fork()) {
      cfd = fd;
      if(fd > 16) {
        printf("Descriptor too large.\n");
        exit(0);
      }
      signal(SIGPIPE,aborting);
      client();
    }
    close(fd);
  }
}

client()
{
  int readfds;
  char buf[1024], address[30], port[10];
  int len=1, i=0, j=0;
  unsigned long addr;
  struct sockaddr_in sin2;
  int s2_flag, cfd_flag, readfds_flag;
  char outbuf[8192];
  FILE *fp;

  ioctl(cfd, FIONBIO, &len);
  fp = fopen(FILENAME, "r");
  if(!fp) aborting();

  fread(outbuf, 8192, 1, fp);
  fclose(fp);

  write_msg(outbuf);
  sleep(4);
  aborting();
}

write_msg(str)
char *str;
{
  write(cfd, str, strlen(str));
}

read_msg(str,slen)
char *str;
int slen;
{
  int readfds;
  int num=0, len;
  char ch;

  do {
    readfds = 1<<cfd;
    while(!select(16, &readfds, 0, 0, 0)) ;
    len = read(cfd, &ch, 1);
    if(len == 0) aborting();
    if(ch == 8 || ch == 127) num--;
    else if(ch == 10 || num==slen-1) ch = 0;
    if(ch != 13 && ch != 8 && ch != 127) str[num++] = ch;
  } while(ch);

}

void aborting(arg)
int arg;
{
  int i=0;

  write_msg("Connection dropped.\n");
  signal(SIGPIPE,SIG_IGN);
  ioctl(cfd, FIONBIO, &i);
  ioctl(s2, FIONBIO, &i);
  close(cfd);
  close(s2);
  exit(0);
}

log(fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char *fmt;
int a1,a2,a3,a4,a5,a6,a7,a8,a9;
{
	FILE *fp;

	if((fp = fopen("/home/Isengard/mordor/log/msg.log", "a")) == NULL) return;
	fprintf(fp, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	fclose(fp);
}

