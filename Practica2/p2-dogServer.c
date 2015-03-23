# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/shm.h>
# include <netinet/in.h> // esta libreria contiene la estructura sockaddr_in
# include <netdb.h>
# include <arpa/inet.h>
# include <strings.h>



#define PORT 9510
#define BACKLOG 32

int *writeFile,*users;  //variable en memoria compartida si esta en 1 es porque estan escribiendo el archivo si esta en 0 esta libre

struct dogType
{
   char nombre[32];
   int edad;
   char raza[16];
   int estatura;
   float peso;
   char  sexo;
};

FILE *file,*newfile, *dogLog;


int isFull();
int crear();
FILE* openFile();
void closeFile();
void atenderCliente(); 
void ingresar();
/*void cargar();*/
void imprimirPerro();
void leer();
void borrar();
void sendPerro();
void recvPerro();
int tamano();

int main(){

	int serverId,clienteId,r,status,shmId;
	struct sockaddr_in  client;
	socklen_t tamano=0;
	key_t key=1234,keyU=3232;
	pid_t pid, end; // identificador de procesos
	char buffer[32]; // prueba de funcionamiento
	serverId=crear();
	shmId=shmget(key,sizeof(int),0666|IPC_CREAT);
	if(shmId<0){
		perror("Error en shmget");
		exit(-1);
	}
	writeFile=(int *)shmat(shmId,0,0);
	*writeFile=0; // estara en 0 si nadie esta escribiendo de lo contrario estara en 1
	shmId=shmget(keyU,sizeof(int),0666|IPC_CREAT);
	if(shmId<0){
		perror("Error en shmget users");
		exit(-1);
	}
	users = (int *)shmat(shmId,0,0);
	*users = 0;
	printf("antes del fork users: %i",*users);
	while(isFull()){
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
			//*users=*users+1;
			printf("\nsoy el hijo # %i ", *users);
			r=recv(clienteId,buffer,7,0 );
			buffer[r]= 0;
			printf("\n Mensaje recibido %s ",buffer);
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
			*users=*users+1;
			printf("\nusuarios padre%i",*users);
	                    
	    }
	    }
	    if(pid!=0){
	    printf("\nEl servidor esta lleno se va adios :");
	    while (*users > 0){
		    end=wait(&status);
		    if(end==-1){
		       perror("\nError al esperar al hijo \n");
		       exit(-1);
		    }
		    if(end > 0){
			    if (WIFEXITED(status))
			    printf("\nChild ended normally\n");
			    else if (WIFSIGNALED(status))
			    printf("\nChild ended because of an uncaught signal\n");
			    else if (WIFSTOPPED(status))
			    printf("\nChild process has stopped\n");
			    printf("\nUsuarios %i",users);
			    *users=*users-1;
		    }
	    }
	    close(clienteId);
	    close(serverId);
	    }else{
	    
	    exit(0); //si el hijo llega aca no tiene nada que hacer
	    }
	    
}

int isFull(){    
    if(*users==BACKLOG){    		
          return 0;		//para parar el while
    }else{
    	printf("is full %i",*users);
          return 1; 	//para continuar el while
    }
}

int crear(){                     //crea el socket del servidor
	printf("\nentro en crear");
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

void atenderCliente(int clientId){     //esta funcion atiende al cliente
	int vivo=0;
	int r,opc=0;
	do{
	printf("\nEntro en atenderCliente\n");	
	r= recv(clientId,&opc,sizeof(int),0);
	if(r<0){
		perror("\n Error al recibir solicitud");
		exit(-1);
	}
	switch(opc){
		case 1 :
		ingresar(clientId);
		break;

		case 2 : 
		leer(clientId);
		break;
		case 3 : 
		borrar(clientId,r);
		break;
		case 4 : printf("Buscar");break;
		case 5 : *users = *users-1;
	        	printf("salio uno %i",*users);
	        	break;
		default : perror ("Opcion invalida");
			  printf("\n%i",opc);
			  opc=0;
			  vivo++;
			  break;
	}
	if(vivo>20){
		*users = *users-1;
	       	printf("salio uno %i",*users);
		perror("El usuario se desconecto repentinamente");
		exit(-1);
	}
	}while(opc!=5);
}

void ingresar(int clientId){
	struct dogType *perros;
	perros= malloc(sizeof(struct dogType));
	printf("\n----------Ingresar Registro----------\n");
	recvPerro(perros,clientId);
	imprimirPerro(perros);
	do{
	if(*writeFile==0){
	*writeFile=1;
	file=openFile("dataDogs.dat");
	int data = fwrite(perros,sizeof(struct dogType),1,file);
	if(data<=0){
		perror("Error de escritura");
	}
	printf("Archivo end\n");
	closeFile(file);
	*writeFile=0;
	free(perros);
	}else{
		printf("Esperando para escribir");
	}
	}while(*writeFile==1);

}


/*void cargar(void *ap ,int clientId){*/
/*  	struct dogType *ingreso;*/
/*  	ingreso = ap; 	*/
/*	recvPerro(ap,clientId);*/
/*  printf("\n Nombre: ");*/
/*  //scanf( " %31[^\n]",ingreso->nombre);*/
/*  r = recv(clientId,ingreso->nombre,32,0);*/
/*  nombre[r]=0;*/
/*  //ingreso->nombre = nombre;*/
/*  printf("\n Edad: ");*/
/*  //scanf(" %d",&ingreso->edad);*/
/*  r = recv(clientId,&ingreso->edad,sizeof(int),0);*/
/*   //= edad;*/
/*  //printf("\n Edad: %i",edad);*/
/*  printf("\n Raza: ");*/
/*  //scanf(" %15[^\n]",ingreso->raza);*/
/*  r = recv(clientId,ingreso->raza,16,0);*/
/*  printf("\n Estatura: ");*/
/*  //scanf(" %i",&ingreso->estatura);*/
/*  r = recv(clientId,&ingreso->estatura,sizeof(int),0);*/
/*  printf("\n Peso: ");*/
/*  //scanf(" %f",&ingreso->peso);*/
/*  r = recv(clientId,&ingreso->peso,sizeof(float),0);*/
/*  printf("\n Sexo M/H: ");*/
/*  //scanf(" %c",&ingreso->sexo);*/
/*  r = recv(clientId,&ingreso->sexo,sizeof(char),0);*/
/*    printf("\n");*/

/*}*/

void leer(int clientId){
	printf("Entro a leer");
	int numeroRegistros = 0,r;
	struct dogType *lectura;
	long tamano=sizeof(struct dogType);
	lectura = malloc(tamano);
	file=openFile("dataDogs.dat");
	fseek(file, 0, SEEK_END);
	numeroRegistros = ftell(file)/tamano;
	//printf("numeroRegistros%i\n",numeroRegistros);
	r = send(clientId,&numeroRegistros, sizeof(int), 0);
	if(r<0){
		perror("error al mandar la cantidad de registros");
		exit(-1);
	}
	int opcion = 0;
	r = recv(clientId,&opcion,sizeof(int),0);
	if(r<0){
		perror("error al recibir el numero  del registro");
		exit(-1);
	}
	printf("Opcion %i\n",opcion );
	if((! numeroRegistros == 0 )&& (fseek(file,opcion*tamano,SEEK_SET)==0)){
		fread(lectura,sizeof(struct dogType),1,file);
		imprimirPerro(lectura);
		sendPerro(lectura,clientId);
	}else{
		printf("\nNo se encontro\n");
	}
	closeFile(file);
	free(lectura);
}

void imprimirPerro(void *ap){
	struct dogType *perros;
	perros = ap;
	printf("\n Nombre: %s",perros->nombre);
	printf("\n Edad: %i",perros->edad);
	printf("\n Raza: %s",perros->raza);
	printf("\n Estatura: %i",perros->estatura);
	printf("\n Peso: %3.2f",perros->peso);
	printf("\n sexo: %c",perros->sexo);
	printf("\n");	
}

void borrar(int clientId,int r){
	
	int found = 0;
	int numeroRegistros = 0;
	struct dogType *perros;
	file = fopen("dataDogs.dat","r+");
	newfile = fopen("temp.dat","w+");
	long tamano=sizeof(struct dogType);
	perros = malloc(tamano);
	fseek(file, 0, SEEK_END);
	numeroRegistros = ftell(file)/tamano;
	r = send(clientId,&numeroRegistros, sizeof(long), 0);
	int opcion = 0;
	r = recv(clientId,&opcion,sizeof(char),0);
	rewind(file);
	printf(".-----------------Borrar-----------%i\n",opcion );
	while (fread(perros,sizeof(struct dogType),1,file) != 0) {
		if (opcion == ftell(file)/tamano-1) {
			printf("Perro Borrado.\n\n");
			found=1;
			
		} else {
			fwrite(perros, sizeof(struct dogType), 1, newfile);
		}
	}

	r = send(clientId,&found, sizeof(int), 0);
	printf("found %i\n", found);

	fclose(file);
	fclose(newfile);

	remove("dataDogs.dat");
	rename("temp.dat", "dataDogs.dat");

	free(perros);


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

void recvPerro(void *ap, int clientId){
    struct dogType *recibiendo;
    recibiendo = ap;
    int r, tam;
    r= recv(clientId,&tam,sizeof(int),0);
    if(r<0){
      perror("Error recv tamano nombre");
      exit(-1);
    }
    r = recv(clientId,recibiendo->nombre,tam,0);
    if(r<0){
  	perror("Error recv nombre");
  	exit(-1);
    }
    r = recv(clientId,&recibiendo->edad,sizeof(int),0);
    if(r<0){
          perror("Error recv edad");
          exit(-1);
    }
    r= recv(clientId,&tam,sizeof(int),0);
    if(r<0){
        perror("Error recv tam raza");
        exit(-1);
    }
    r = recv(clientId,recibiendo->raza,tam,0);
    if(r<0){
         perror("Error recv raza");
         exit(-1);
    }
    r = recv(clientId,&recibiendo->estatura,sizeof(int),0);
    if(r<0){
        perror("Error recv estatura");
        exit(-1);
    }
    r = recv(clientId,&recibiendo->peso,sizeof(float),0);
    if(r<0){
           perror("Error recv peso");
           exit(-1);
    }
    r = recv(clientId,&recibiendo->sexo,sizeof(char),0);
    if(r<0){
            perror("Error recv sexo");
                    exit(-1);
    }
}
void sendPerro(void *ap,int clientId){
  struct dogType *lectura;
  lectura = ap;
  int r, tam;
  tam=tamano(lectura->nombre);
  r=send(clientId,&tam,sizeof(int),0);
  if(r<0){
  perror("error en send tam nombre");
  exit(-1);
  }
  r=send(clientId,lectura->nombre,tam,0);
  if(r<0){
    perror("error en send nombre");
    exit(-1);
  }
  r=send(clientId,&lectura->edad,sizeof(int),0);
  if(r<0){
    perror("error en send edad");
    exit(-1);
  }
  tam=tamano(lectura->raza);
  r=send(clientId,&tam,sizeof(int),0);
  if(r<0){
    perror("error en send tam raza");
    exit(-1);
  }
  r=send(clientId,lectura->raza,tam,0);
  if(r<0){
     perror("error en send raza");
     exit(-1);
  }
  r=send(clientId,&lectura->estatura,sizeof(int),0);
  if(r<0){
    perror("error en send estatura");
    exit(-1);
  }
  r=send(clientId,&lectura->peso,sizeof(float),0);
  if(r<0){
     perror("error en send peso");
     exit(-1);
  }
  r=send(clientId,&lectura->sexo,sizeof(char),0);
  if(r<0){
    perror("error en send sexo");
    exit(-1);
  }
}
int  tamano(char palabra[]){
	int i=0;
	while(palabra[i]!='\0')
	i++;
	return i;
}

