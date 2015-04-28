#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
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

	struct hostent *webinfo = NULL;
	SOCKADDR_IN webSin = { 0 };
	const char *webip = "208.97.177.124";
	char buffer[512];
	char webBuffer[512];
	int n = 0;
	SOCKET webSock = socket(AF_INET,SOCK_STREAM,0);
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
	printf("Hôte connecté\n");
	if(csock == INVALID_SOCKET)
	{
		perror("accept()");
		exit(1);

	}

	printf("Requête reçue\n");

	if((n = read(csock, buffer, sizeof buffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	if(webSock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}
	if((n = (csock, buffer, sizeof buffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}

	printf("Hôte trouvé\n");
	webinfo = gethostbyname(webip);
	if (webinfo == NULL)
	{
		fprintf (stderr, "Unknown host ");
		exit(EXIT_FAILURE);
	}
	webSin.sin_addr = *(IN_ADDR *) webinfo->h_addr;
	webSin.sin_port = htons(80);
	webSin.sin_family = AF_INET;

	if(connect(webSock,(SOCKADDR *) &webSin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}
	printf("Connecté\n");
	if(n = (write(webSock, buffer, strlen(buffer)) < 0))
	{
		perror("send()");
		exit(errno);
	}
	printf("Requête envoyée\n");
	if((n = read(webSock,webBuffer,sizeof webBuffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	puts(webBuffer);
	printf("Page web Chargée\n");
	closesocket(sock);
	closesocket(webSock);




	return 0;
}
