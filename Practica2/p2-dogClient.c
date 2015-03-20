#include<stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>

#define PORT 3141
int conectar();
int main( int argc,char *argv[]){

	int clienteId, r;
	char buffer[32];
	clienteId=conectar(argv[1]);
	r= send(clienteId,"Hola mundo ",11,0);
	if(r<0){
		perror("\nError en send(): ");
		exit(-1);
	}
	r=recv(clienteId,buffer,32,0);
	buffer[r]=0;
	printf("\n Mensaje recibido: %s",buffer);
	close(clienteId);

}

int conectar( char *argv){
	struct sockaddr_in cliente;
	int clienteId, r;
	clienteId = socket(AF_INET,SOCK_STREAM,0);
	if(clienteId<0){
		perror("\n Error en socket():\n");
		exit(-1);
	}
	cliente.sin_family = AF_INET;
	cliente.sin_port = htons(PORT);
	
	inet_aton(argv,&cliente.sin_addr);
	
	r = connect(clienteId,(struct sockaddr *)&cliente,(socklen_t)sizeof(struct sockaddr));
	if(r<0){
		perror("\n Error en connect(): \n");
		exit(-1);
	}
	return clienteId;
}
