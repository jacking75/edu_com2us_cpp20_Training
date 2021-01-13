/* 서비스 중지할때 쓰는거예요.. */


#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
main()
{

	char buffer[80];
	int sockfd, new_sockfd, clilen;
	struct sockaddr_in cli_addr, serv_addr;

	if(fork()) exit(0);
	close(0);close(1);close(2);

	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(4000);
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);
	while(1) {
		new_sockfd=accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		bzero(buffer, 80);
		strcpy(buffer, "\n- 무 한 대 전 -\n\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "7일 서버가 전원이 꺼진 관계로 잠시 서비스가 중지됩니다.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "불편을 드려 죄송합니다.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "특별히 전하실 내용이 있으시면 그루터기 BBS 의 머드게시판에 써 주십시오.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "그루터기 BBS : ksb.kumoh.ac.kr   login:bbs\n\n");
		write(new_sockfd, buffer, strlen(buffer));
        close(new_sockfd);
	}
}
