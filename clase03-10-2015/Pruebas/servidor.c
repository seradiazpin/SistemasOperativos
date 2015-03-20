#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 3535
#define BACKLOG 5

/*
#include <netinet/in.h>

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

*/
int main(){

    int serverfd, clientfd, r, opt = 1,isFull = 0;
    struct sockaddr_in server, client;
    socklen_t tamano = 0;
    pid_t pid;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd < 0){
        perror("\n-->Error en socket():");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8); 

    setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if(r < 0){
        perror("\n-->Error en bind(): ");
        exit(-1);
    }

    r = listen(serverfd, BACKLOG);
    if(r < 0){
        perror("\n-->Error en Listen(): ");
        exit(-1);
    }

    static int users = 0;

    while (!isFull)
   	{
	    clientfd = accept(serverfd, (struct sockaddr *)&client, &tamano); //accept es una funcion bloquenate se queda esperando a que alguien se conencte
	    if(clientfd < 0)
	    {
	        perror("\n-->Error en accept: ");
	        exit(-1);
	    }

	    pid = fork();
	    if (pid < 0){
	         perror("ERROR on fork");
	         exit(1);
	    }
	    if (pid == 0){
	            r = send(clientfd, "hola mundo", 10, 0);
	            users++;
	            if(r < 0){
		            perror("\n-->Error en send(): ");
		            exit(-1);
		        }
	        close(serverfd);
	        exit(0);

	    }else{
	    	 users++;
	         printf("Users:%i\n",users);
	         close(clientfd);

	         if(users > BACKLOG){
	         	isFull = 1;
	         }
    }

   }

    close(client);
    close(serverfd);
}
