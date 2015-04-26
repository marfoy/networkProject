#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#define closesocket(s) close(s)
#define PORT 80
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define errno -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


int main(){
	char recvBuffer[1024];
	int n;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	char data[4] = "Test";
	char buffer[512] ="POST / HTTP/1.1\r\nHost: www.google.be\r\nConnection: Close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 0\r\n\r\n";
	if(sock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}

	struct hostent *hostinfo = NULL;
	SOCKADDR_IN sin = { 0 };
	const char *hostname = "208.97.177.124";



	hostinfo = gethostbyname(hostname);
	if (hostinfo == NULL)
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
	sin.sin_port = htons(PORT);
	sin.sin_family = AF_INET;

	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}

	if(n = (send(sock, buffer, 10, 0)) < 0)
	{
		perror("send()");
		exit(errno);
	}
	if((n = recv(sock,recvBuffer,sizeof buffer - 1,0)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	printf("%s",recvBuffer);	
	closesocket(sock);



}
