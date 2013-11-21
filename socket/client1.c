#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SERVPORT 5555
#define DEST_IP "192.168.1.101"
char sendline[20],recvline[20];  
int n;
int main(int argc, char **argv)
{
	int sockfd,sock_dt;
	printf("#####################################################\n");
	printf("socket test      by pafone   19th,April,2009\n");
	printf("#####################################################\n");
	struct sockaddr_in my_addr;//local ip info
	struct sockaddr_in dest_addr; //destnation ip info
	if(argc != 3)
	{
		printf("useage:socket_client ipaddress port\n");
		printf("eg:socket_client:192.168.1.101 5555\n");
		return -1;
	}
	int destport = atoi(argv[2]);
	if(-1 == (sockfd = socket(AF_INET,SOCK_STREAM,0)) )
	{
		perror("error in create socket\n");
		return 0;
	}
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
	//    bzero(&dest_addr.sin_zero,0,8);
	memset(&dest_addr.sin_zero,0,8);
	//connect
	if(-1 == connect(sockfd,(struct sockaddr*)&dest_addr,sizeof(struct sockaddr)))
	{
		perror("connect error\n");
		return 0;
	}
	int n_send_len;

	while(1) {
		n_send_len = send(sockfd,"1234567890",10,0);
		sleep(1);
//		printf("%d bytes sent\n",n_send_len);
		printf("waiting for server...\n");  
		memset(recvline,0xaa, 20);
		n=recv(sockfd,recvline,100,0);  
		recvline[n]='\0';  
		printf("recv data is:%s\n",recvline);  
	}
	close(sockfd);
}
