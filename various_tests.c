#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int main(){
	char buffer[512] = "GET www.google.be HTTP/1.1\r\nHost: www.google.be\r\nConnection: Close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 0\r\n\r\n";
	char buffer2[512];
	char buffer3[] = "chien////'3é&§§ et çç chat";
	char *request = strtok(buffer, "\n\r");
	char *get;
	int i=0;

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
