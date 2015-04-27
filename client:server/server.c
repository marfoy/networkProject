/*
 * SERVER
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     
     // Check executable args
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     // Create the server socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     // If error opening socket
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     // 
     bzero((char *) &serv_addr, sizeof(serv_addr));
     
     // Get port # in int
     portno = atoi(argv[1]);
     
     //
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     // Bind socket
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     // Start listening
     listen(sockfd,5);
     
     // 
     clilen = sizeof(cli_addr);
     
     // Accept a client
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     
     // Error acccepting
     if (newsockfd < 0) 
          error("ERROR on accept");
     
     // Reset buffer
     bzero(buffer,256);
     
     // Read bytes from socket
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     
     // Calculate
     int a, b;
     char operator;
     sscanf(buffer, "%d%c%d", &a, &operator, &b);
     
     int result;
     switch (operator)
     {
		 case '+':
			 result = a + b;
		 break;
		 case '-':
			 result = a - b;
		 break;
		 case '*':
			 result = a * b;
		 break;
		 case '/':
			 result = a / b;
		 break;
		 default:
		 break;
	 }
     
     // Display result
     printf("Here is the result: %d\n", result);
     
     // Send confirmation
     n = write(newsockfd,"Received",18);
     if (n < 0) error("ERROR writing to socket");
     
     // Close
     close(newsockfd);
     close(sockfd);
     
     return 0; 
}
