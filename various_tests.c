#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Place dans le buffer la chaine de caractere qu'il y a entre begin et end.
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
	char buffer[512] = "GET www.google.be HTTP/1.1\r\nHost: www.google.be\r\nConnection: Close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 0\r\n\r\n";
	char buffer2[512];
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

 	/*
    httpend = " HTTP";
    end = "\r\n";
    get = strtok_r(buffer, "GET ", &httpend);
    host = strtok_r(buffer, "Host: ", &end);

    printf("%s\n", get);
    printf("%s\n", host); */

	return 0; 
}
