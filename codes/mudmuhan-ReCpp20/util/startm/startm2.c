#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct termio mbuf,systerm;
void main()
{
int OUT,MUDID = -1;

if(fork()) exit(0);
close(0);close(1);close(2);
 
       while(1) {
          OUT = getpgid(MUDID);
          if(OUT==-1) {
                  if(fork()) { 
       			ioctl(0, TCSETAF, &systerm);
       		  	system("/home/mordor/bin/frp -r");
       			ioctl(0, TCSETAF, &mbuf);
                        MUDID = getpid()+1;
                        exit();
                   }
          	}

           		sleep(30);
       }
}
