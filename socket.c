#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 4242
#define errno -1
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;



int main(){

	char buffer[10];
	int n = 0;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN csin = { 0 };
	SOCKET csock;
	int sinsize = sizeof csin;

	if(sock == INVALID_SOCKET){

		perror("socket()");
		exit(1);

	}

	SOCKADDR_IN sin = { 0 };

	sin.sin_addr.s_addr = htonl(INADDR_ANY); //N'importe quelle adresse est accéptée car Serveur
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	if ( bind( sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
		perror("blind()");
		exit(1);
	}

	if( listen(sock, 5) == SOCKET_ERROR)
	{
		perror("listen()");
		exit(1);
	}

	csock = accept(sock, (SOCKADDR *) &csin, &sinsize);

	if(csock == INVALID_SOCKET)
	{
		perror("accept()");
		exit(1);

	}

	if((n = recv(csock, buffer, sizeof buffer - 1, 0)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	printf("%s\n",buffer);
	closesocket(sock);
	closesocket(csock);




	return 0;
}
