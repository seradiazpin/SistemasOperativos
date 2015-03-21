# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <netinet/in.h> // esta libreria contiene la estructura sockaddr_in
# include <netdb.h>
# include <arpa/inet.h>
# include <strings.h>

# include <unistd.h>// esta es para el sleep

#define PORT 9510
#define BACKLOG 32

struct dogType
{
       char nombre[32];
       int edad;
       char raza[16];
       int estatura;
       float peso;
       char  sexo;
};

FILE *file, *dogLog;


int isFull();
int crear();
FILE* openFile();
void closeFile();
void atenderCliente(); 


int main(){

	int serverId,clienteId,r, users=0,status;
	struct sockaddr_in  client;
	socklen_t tamano=0;
	pid_t pid, end; // identificador de procesos
	char buffer[32]; // prueba de funcionamiento
	serverId=crear();
	while(isFull(users)){
		clienteId=accept(serverId,(struct sockaddr *)&client,&tamano);
		if(clienteId<0)
		{
			perror("\n Error en accept: \n");
			exit(-1);
		}
		pid =fork();
		if ( pid < 0 ){
			perror("\n Error en fork: ");
			exit(-1);
		}
		if(pid==0){ //Somos hijos
			printf("soy el hijo # %i ",users);
			r=recv(clienteId,buffer,32,0 );
			buffer[r]=0;
			printf("\n Mensaje recibido %s",buffer);
			r = send(clienteId, "hola mundo", 10, 0);
                        if(r < 0){
                               perror("\n-->Error en send(): ");
                               exit(-1);
                        }
                        //atender usuario
                        atenderCliente(clienteId);
                        
			close(clienteId);
	                close(serverId);
	                exit(0);
		}else{	//soy el padre
			users++;
		//	do{
			end=waitpid(-1,&status,WUNTRACED|WNOHANG |WCONTINUED); // aca espera al hijo para continuar
			if(end==-1){
				perror("\nError al esperar al hijo \n");
				exit(-1);
			}
		//	}while(end==0);
			printf("usuarios %i",users);
			if(end > 0){
			if (WIFEXITED(status))
			      printf("\nChild ended normally\n");
                	 else if (WIFSIGNALED(status))
                              printf("Child ended because of an uncaught signal\n");
                      	 else if (WIFSTOPPED(status))
                              printf("Child process has stopped\n");
                       printf("Usuarios %i",users);
                        users--;
                        }
                        
                }
        }
        printf("\nEl servidor esta lleno se va adios :");
        while (users > 0){
        end=wait(&status);
        if(end==-1){
           perror("\nError al esperar al hijo \n");
           exit(-1);
         }
        if(end > 0){
        if (WIFEXITED(status))
        printf("\nChild ended normally\n");
        else if (WIFSIGNALED(status))
        printf("Child ended because of an uncaught signal\n");
        else if (WIFSTOPPED(status))
        printf("Child process has stopped\n");
        printf("Usuarios %i",users);
        users--;
        }
        }
        close(clienteId);
        close(serverId);
}
int isFull(int users){
        if(users==BACKLOG){
              return 0;		//para parar el while
        }else{
              return 1; 	//para continuar el while
        }
 }

int crear(){
	printf("entro en crear");
	int serverId, opt=1,r;
	struct sockaddr_in server, client;
	serverId=socket(AF_INET,SOCK_STREAM,0);	
	if(serverId<0){
		perror("\nError en socket()\n");
		exit(-1);
	}
	server.sin_family = AF_INET;	
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8); // pertenece a la libreria strings.h
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
	return serverId;
}


FILE * openFile(char *nombre){  //metodo para abrir los archivos
	file= fopen(nombre,"a+");
	if(file==NULL){
		perror ("\nError al abrir el archivo para escritura");
		exit(-1);
	}else{
		return file;
	}

}

void closeFile(FILE  *file){   //metodo para cerrar los archivos
	if(!fclose(file)==0){
		perror("\nError al cerrar el archivo");
		exit(-1);
	}
}

void atenderCliente(int clientId){
	int r,opc;
	r= recv(clientId,&opc,sizeof(int),0);
	if(r<0){
		perror("\n Error al recibir solicitud");
		exit(-1);
	}
	switch(opc){
		case 1 : printf("Ingresar");break;
		case 2 : printf("Leer");break;
		case 3 : printf("Borrar");break;
		case 4 : printf("Buscar");break;
		default : perror ("Opcion invalida");
			break;
	}


}
