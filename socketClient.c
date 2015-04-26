#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#define closesocket(s) close(s)
#define PORT 4242
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define errno -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


int main(){
    int n;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[10] = "Hello";
    if(sock == INVALID_SOCKET)
    {
    	perror("socket()");
    	exit(errno);
    }

    struct hostent *hostinfo = NULL;
    SOCKADDR_IN sin = { 0 };
    const char *hostname = "192.168.1.18";

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
    closesocket(sock);



}
