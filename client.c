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
#define PORT 4242
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define errno -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

/*

	CLIENT TEST 

*/


int main(){
	char recvBuffer[1024];
	int n;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	char data[4] = "Test";
	char buffer[512] ="GET / HTTP/1.1\r\nHost: www.perdu.com\r\n\r\n";
	if(sock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}
	printf("Initialisation\n");
	struct hostent *hostinfo = NULL;
	SOCKADDR_IN sin = { 0 };
	const char *hostname = "10.102.26.28";


	printf("Hôte trouvé\n");
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
	printf("Connecté\n");
	if(n = (write(sock, buffer, strlen(buffer)) < 0))
	{
		perror("send()");
		exit(errno);
	}
	printf("Requête envoyée\n");
	if((n = read(sock,recvBuffer,sizeof recvBuffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	puts(recvBuffer);
	printf("Recepetion terminée\n");
	printf("%s",recvBuffer);	
	closesocket(sock);
	return 1;
}
