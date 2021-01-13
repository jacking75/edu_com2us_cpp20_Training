#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct termio mbuf,systerm;
void main()
{
if(fork()) exit(0);
close(0);close(1);close(2);

       while(1) {
           ioctl(0, TCSETAF, &systerm);
           system("/home/muhan/bin/frp -r");
           ioctl(0, TCSETAF, &mbuf);

           sleep(30);
       }
}
