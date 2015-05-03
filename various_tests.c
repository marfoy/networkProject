#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

//Place dans le buffer la chaine de caractere qu'il y a entre begin et end.
void research(char *str, char *begin, char *end, char *buffer){
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

//filter a string with only alphanumeric character in the destination string.
void filter(char *name){
	char *src, *dst;
	for (src = name, dst = src; *src; src++) {
   		if ('a' <= *src && *src <= 'z' 
    	|| '0' <= *src && *src <= '9' 
    	|| *src == '_') *dst++ = *src;
	}
	*dst = '\0';
}

//retourne l'heure actuelle de la machine (chaine de caractere)
char * getHour(){
	struct tm* gmtime (const time_t *temps);
	time_t temps;
	struct tm date;
	char *result = (char *) malloc(sizeof(char) * 32);

	time(&temps);
    date=*gmtime(&temps);
    sprintf(result, "%d:%d",(date.tm_hour+2+24)%24, date.tm_min);

    return result;
    
}

//retourne l'heure actuelle de la machine en secondes
int getTime(){
	time_t secondes;
    struct tm now;
    int x;

    time(&secondes);
    now=*localtime(&secondes);

    x = now.tm_hour*3600 + now.tm_min*60 + now.tm_sec;
    return x;
}

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
					//toDo : recharger la page.
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

int main(){
	char buffer[512] = "GET www.google.be HTTP/1.1\r\nHost: www.google.be\r\nConnection: Close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 0\r\n\r\n";
	char buffer2[512];
	char buffer3[] = "chien////'3é&§§ et çç chat";
	char *request = strtok(buffer, "\n\r");
	char *get;
	int i=0;

	check(request);
	while( request != NULL )
	{
		if(i==0){
			research(request, "GET ", " HTTP/1.1", buffer2);
 			printf("\n%s\n", buffer2);
		}
		if(i==1){
    		research(request, "Host: ", "\r\n", buffer2);
 			printf("\n%s\n", buffer2);
 		}

 		i++;
    	request = strtok(NULL, "\n\r");
    }

    printf("Heure : %s\n", getHour());
    filter(buffer3);
    printf("%s\n", buffer3);
    printf("%d\n",getTime());

 	/*
    httpend = " HTTP";
    end = "\r\n";
    get = strtok_r(buffer, "GET ", &httpend);
    host = strtok_r(buffer, "Host: ", &end);

    printf("%s\n", get);
    printf("%s\n", host); */

	return 0; 
}
