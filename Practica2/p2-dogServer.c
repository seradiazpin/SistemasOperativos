# include <stdio.h>
# include <netienet/in.h> // esta libreria contiene la estructura sockaddr_in
# include <sys/socket.h> // contiene a socklen_t



#define PORT 3141

int main(){

	int serverId,
	struct sockaddr_in server, client;
	socklen_t tamano;
	
	serverId=socket(AF_INET,SOCK_STREAM,0);
	if(serverId<0){
		perror("\nError en socket()\n");
		exit(-1);
	}
	
	
	

}
