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


//Methode qui rehcherche les informations utiles dans ma requête
void research(char *str, char *begin, char *end, char *buffer){
	char *first = strstr(str,begin);
	if(first != NULL){
		char *last = strstr(first,end);
		printf("First : %s \nLast : %s \n",first,last);
		first += strlen(begin);
		if(last != NULL){
			while(first != last){
				buffer[0] = first[0];
				buffer++;
				first++;
			}
		}
		else{
			while(strlen(first)>0){
				buffer[0] = first[0];
				buffer++;
				first++;
			}
		}
	}
	buffer[0] = '\0';
}
int main(){
	//Recupération de toutes les pages web en memoire dans le dossier courant 
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
	//Creation du socket de connexion avec le client 
	if(sock == INVALID_SOCKET){

		perror("socket()");
		exit(1);

	}

	SOCKADDR_IN sin = { 0 };

	sin.sin_addr.s_addr = htonl(INADDR_ANY); //N'importe quelle adresse est accéptée car Serveur
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	//Connexion avec le client
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
	//Creation du socket avec client pour le transfert de donnée
	csock = accept(sock, (SOCKADDR *) &csin, &sinsize);
	if(csock == INVALID_SOCKET)
	{
		perror("accept()");
		exit(1);

	}

	//Lecture de la requête
	if((n = read(csock, buffer, sizeof buffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	//Recupération des informations de la page demandée
	char copyBuffer[strlen(buffer)];
	strcpy(copyBuffer,buffer);
	char page[512];
	char host[512];
	char *request = strtok(copyBuffer, "\n\r");
	char *get;
	int i=0;

	while( request != NULL )
	{
		if(i==0){
			research(request, "GET ", " HTTP/1.1", page);
 			printf("\n%s\n", page);
		}
		if(i==1){
    		research(request, "Host: ", "\r\n", host);
 			printf("\n%s\n", host);
 		}

 		i++;
    	request = strtok(NULL, "\n\r");
    	}
	//Verification si la page demandée n'est pas en memoire
	

	/*Creation de socket de connexion avec le serveur web 
	lorsque la page demandée n'est pas en cache*/
	if(webSock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}
	
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

	//Envoi de la requête au serveur web
	if(n = (write(webSock, buffer, strlen(buffer)) < 0))
	{
		perror("send()");
		exit(errno);
	}
	puts(buffer);
	//Lecture de la réponse du serveur web
	if((n = read(webSock,webBuffer,sizeof webBuffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}

	//Transfert du contenu html au client
	if(n = (write(csock, webBuffer, strlen(webBuffer)) < 0))
	{
		perror("send()");
		exit(errno);
	}

	closesocket(csock);
	closesocket(sock);
	closesocket(webSock);

	return 0;
}
