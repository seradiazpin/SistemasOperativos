# include <stdio.h>
# include <netienet/in.h> // esta libreria contiene la estructura sockaddr_in
# include <sys/socket.h> // contiene a socklen_t



#define PORT 3141
#define BACKLOG 32

int main(){

	int serverId,clienteId,r,opt = 1;
	struct sockaddr_in server, client;
	socklen_t tamano;
	
	serverId=socket(AF_INET,SOCK_STREAM,0);
	if(serverId<0){
		perror("\nError en socket()\n");
		exit(-1);
	}
	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.saddr = INADDR_ANY;
	bzero(server.sin_zeo,8); // pertenece a la libreria strings.h
	
	setsockopt(serverId,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int)); //No se que hace
	
	r=bind(serverId,(struct sockaddr *)&server, sizeof(struct sockaddr)); //le da direccion al socket
	
	if(r<0){
		perror("\nError en bind():\n");
		exit(-1);
	}
	
	r= listen(serverId, BACKLOG);
	if(r<0){
		perror("\nError en listen():\n");
		exit(-1);
	}

}
