/* ���� �����Ҷ� ���°ſ���.. */


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
		strcpy(buffer, "\n- �� �� �� �� -\n\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "7�� ������ ������ ���� ����� ��� ���񽺰� �����˴ϴ�.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "������ ��� �˼��մϴ�.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "Ư���� ���Ͻ� ������ �����ø� �׷��ͱ� BBS �� �ӵ�Խ��ǿ� �� �ֽʽÿ�.\n");
		write(new_sockfd, buffer, strlen(buffer));
		bzero(buffer, 80);
		strcpy(buffer, "�׷��ͱ� BBS : ksb.kumoh.ac.kr   login:bbs\n\n");
		write(new_sockfd, buffer, strlen(buffer));
        close(new_sockfd);
	}
}
