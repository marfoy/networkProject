#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#define MAX_CLIENTS 25
#define BUF_SIZE 50
#define SIZEMAX 512
#define PORT 4242
#define errno -1
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

/**
Ce maillon de liste chainee contient les informations suivantes :
seconds : le moment en secondes ou la page a ete enregistree dans le cache.
fileName : le nom de la page.
next : le pointeur vers le maillon suivant.
**/
typedef struct node *nodePointer;
struct node{
	int seconds;
	char *fileName;
	nodePointer next;
};

int in(nodePointer head, nodePointer tail, char *file){
	nodePointer currentNode = head;
	while(1){
		if(strcmp(currentNode->fileName,file)==0){
			return 1;
		}
		if(currentNode == tail){
			return 0;
		}
		currentNode = currentNode->next;
	}
}

/**
verifie si une page chargee l'a ete avec un temps > 5 min
@param head : pointeur vers la tete de la liste chainee associant les pages chargees avec le moment ou elles ont ete chargees pour la derniere fois
*/
void * check(void *argv){
	struct node n1;
	nodePointer head = &(n1), tail, currentNode;
	int i=0; //pour verifier si on est dans la premiere iteration (pour initialiser la liste chainee)
	DIR *d;
	struct dirent *dir;

	while(1){
		currentNode = head;
		d = opendir(".");
		if(i==0){
			//initialisation de la liste ; on est toujours dans cette boucle tant qu'aucune requête n'a ete faite.
			if (d){
    			while ((dir = readdir(d)) != NULL){
    				int len = strlen(dir->d_name);
    				if(dir->d_name[len-5]=='.' && dir->d_name[len-4]=='h' 
    					&& dir->d_name[len-3]=='t' && dir->d_name[len-2]=='m' && dir->d_name[len-1] == 'l'){
      					char *str = (char *) malloc(sizeof(char)*(len+1));
      					strcpy(str,dir->d_name);
      					if(i>0){
      						nodePointer nextNode;
      						nextNode = (nodePointer) malloc(sizeof(struct node));
      						currentNode->next = nextNode;
      						currentNode = currentNode->next;
      						currentNode->fileName = str;
      						currentNode->seconds = getTime();
      					}
      					else{
      						//traitement de la tete de la liste
      						currentNode->fileName = str;
      						currentNode->seconds = getTime();
      					}
      					printf("%s is loaded.\n", currentNode->fileName);
      					i++;
      				}
    			}
    			tail = currentNode;
    			printf("the head is : %s at %d\n",head->fileName,head->seconds);
    			printf("the tail is : %s at %d\n",tail->fileName,tail->seconds);
    			printf("\n");
			}
		}
		else{
			currentNode = head;
			if(d){
				while ((dir = readdir(d)) != NULL){
					int len = strlen(dir->d_name);
    				if(len > 5 && dir->d_name[len-5]=='.' && dir->d_name[len-4]=='h' 
    					&& dir->d_name[len-3]=='t' && dir->d_name[len-2]=='m' && dir->d_name[len-1] == 'l'){
						if(in(head, tail, dir->d_name) == 0){
							printf("%s is not in list. So it is now added in.\n", dir->d_name);
							//si le fichier n'est pas present, l'ajouter dans la liste.
							char *str = (char *) malloc(sizeof(char)*(len+1));
							strcpy(str,dir->d_name);
							nodePointer nextNode = (nodePointer) malloc(sizeof(struct node));
							nextNode->fileName = str;
							nextNode->seconds = getTime();
							tail->next = nextNode;
							tail = nextNode;

							printf("the head is : %s at %d\n",head->fileName,head->seconds);
							printf("the tail is : %s at %d\n",tail->fileName,tail->seconds);
							printf("\n");
							printf("this is the new list : \n");
							while(1){
								printf("-> %s, -> %d\n", currentNode->fileName, currentNode->seconds);
								if(currentNode==tail){
									currentNode = head;
									break;
								}
								currentNode = currentNode->next;
							}
							printf("\n");
						}
					}
				}
			}
			while(1){
				
				if(getTime()-currentNode->seconds > 40){
					/*FILE *file = NULL;
					file = fopen(currentNode->fileName,"r");
					char line[SIZEMAX];
					if(file != NULL){
						while(fgets(line,SIZEMAX,file) != NULL){
							strcat(webBuffer,line);
						}
						fclose(file);
					}*/
					printf("the page %s must be reloaded (last call : %d )\n", currentNode->fileName, currentNode->seconds);
					currentNode->seconds = getTime();
				}
				if(currentNode == tail){
					break; //Le while(currentNode != NULL) provoque une Segmentation Fault...
				}
				currentNode = currentNode->next;
			}
		}
		closedir(d);
	}
}

void filter(char *name){
	char *src, *dst;
	for (src = name, dst = src; *src; src++) {
   		if ('a' <= *src && *src <= 'z' 
    	|| '0' <= *src && *src <= '9' 
    	|| *src == '_') *dst++ = *src;
	}
	*dst = '\0';
}

char * getHour(){
	struct tm* gmtime (const time_t *temps);
	time_t temps;
	struct tm date;
	char *result = (char *) malloc(sizeof(char) * 32);

	time(&temps);
	date=*gmtime(&temps);
	sprintf(result, "%d:%d",(date.tm_hour+2+24)%24, date.tm_min);
	printf("%s\n",result);

	return result;

}
int isAlreadySave(char *file){
	DIR * rep = opendir(".");
	if(rep != NULL){
		struct dirent *dir;
		while((dir = readdir(rep)) != NULL){
			if(strcmp(dir->d_name,file) == 0){
				return 1;
			}
		}	
		closedir(rep);

	}
	return 0;


}
//Methode qui rehcherche les informations utiles dans ma requête
void research(char *str, char *begin, char *end,char *buffer){
	char *first = strstr(str,begin);
	if(first != NULL){
		char *last = strstr(first,end);
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
void* clientProcessing(void *arg){
	SOCKET csock = (SOCKET) arg;
	char buffer[512];
	char webBuffer[512];
	int n = 0;
	SOCKET webSock = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN webSin = { 0 };
	struct hostent *webinfo = NULL;
	//Creation du socket avec client pour le transfert de donnée

	//Lecture de la requête
	if((n = read(csock, buffer, sizeof buffer)) < 0)
	{
		perror("recv()");
		exit(errno);
	}
	//Recupération des informations de la page demandée
	char copyBuffer[strlen(buffer)];
	strcpy(copyBuffer,buffer);
	char page[256];
	char host[256];
	char webIp[256];
	char *request = strtok(copyBuffer, "\n\r");
	char *get;
	int i=0;
	while( request != NULL )
	{
		if(i==0){
			research(request, "GET ", " HTTP/1.1", page);
		}
		if(i==1){
			research(request, "Host: ", "\r\n", host);
		}

		i++;
		request = strtok(NULL, "\n\r");
	}
	/*Creation de socket de connexion avec le serveur web 
	lorsque la page demandée n'est pas en cache*/
	strcpy(webIp,host);
	strcat(host,page);
	filter(host);
	strcat(host,".html");
	if(!isAlreadySave(host)){
		printf("Page non sauvée\n");

		if(webSock == INVALID_SOCKET)
		{
			perror("socket()");
			exit(errno);
		}
		
		webinfo = gethostbyname(webIp);
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
		FILE *file = NULL;
		file = fopen(host,"w");
		if(file != NULL){
			strcat(webIp," HOST ");
			strcat(page," PAGE ");
			strcat(webBuffer,webIp);
			strcat(webBuffer,page);
			fputs(webBuffer,file);
			fclose(file);
		}
	}
	else{
		//récupération de la page html
		printf("recupération du fichier\n");
		FILE *file = NULL;
		file = fopen(host,"r");
		char line[SIZEMAX];
		if(file != NULL){
			while(fgets(line,SIZEMAX,file) != NULL){
				strcat(webBuffer,line);
			}
			fclose(file);
		}
	}


	//Transfert du contenu html au client
	if(n = (write(csock, webBuffer, strlen(webBuffer)) < 0))
	{
		perror("send()");
		exit(errno);
	}

	closesocket(webSock);
}
int main(){

	//Creation du socket de connexion avec le client 
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET){

		perror("socket()");
		exit(1);

	}

	SOCKADDR_IN sin = { 0 };
	sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	//Connexion avec le client
	if ( bind( sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
		perror("blind()");
		exit(1);
	}

	if( listen(sock, 10) == SOCKET_ERROR)
	{
		perror("listen()");
		exit(1);
	}
	while(1){
		pthread_t id;
		SOCKADDR_IN csin = { 0 };
		SOCKET csock;
		int sinsize = sizeof csin;
		csock = accept(sock, (SOCKADDR *) &csin, &sinsize);
		if(csock == INVALID_SOCKET)
		{
			perror("accept()");
			exit(1);

		}
		pthread_create(&id,NULL,clientProcessing,(void*) csock);
	}
	return 0;
}
