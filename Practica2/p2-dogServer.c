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
# include <string.h>
# include <time.h>

#define PORT 9510
#define BACKLOG 31

struct dogType
{
   char nombre[32];
   int edad;
   char raza[16];
   int estatura;
   float peso;
   char  sexo;
};

int *writeFile,*users;  //variable en memoria compartida si esta en 1 es porque estan escribiendo el archivo si esta en 0 esta libre

int isFull();
int crear();
FILE* openFileR();
FILE* openFileA();
void fileEnd();
void atenderCliente(); 
void ingresar();
void imprimirPerro();
void leer();
void borrar();
void buscar();
void sendPerro();
void recvPerro();
int tamano();
void minToMay();
int numRegs();
void writeLog();

int main(){
	int serverId,clienteId,r,status,shmId,userTmp;
	struct sockaddr_in  client;
	socklen_t tamano=0;
	key_t key=1234,keyU=3232;
	pid_t pid, end; // identificador de procesos
	char buffer[32], *ipAddr; // prueba de 	funcionamiento
	serverId=crear();
	shmId=shmget(key,sizeof(int),0666|IPC_CREAT);	//Creacion del espacio en memoria compartida
	if(shmId<0){
		perror("Error en shmget");
		exit(-1);
	}
	writeFile=(int *)shmat(shmId,0,0);  //Asosiacion del espacio de memoria compartida
	*writeFile=0; // estara en 0 si nadie esta escribiendo de lo contrario estara en 1
	shmId=shmget(keyU,sizeof(int),0666|IPC_CREAT);//Espacio de memoria para el numero de usuarios
	if(shmId<0){
		perror("Error en shmget users");
		exit(-1);
	}
	users = (int *)shmat(shmId,0,0);
	*users = 0;
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
			ipAddr=inet_ntoa(client.sin_addr);
			printf("\nsoy el hijo # %i %s :: %d \n",*users,ipAddr,ntohs(client.sin_port)); //Prueba para el Log	        */
	      		atenderCliente(clienteId,ipAddr);  //atender usuario
			close(clienteId);
	        	close(serverId);
	        	exit(0);
		}else{	//soy el padre
			if(*users<userTmp){		// si el numero de usuarios es menor al que estaba anteriormente espera hasta que esos hijos mueran para continuar
			    int dead=0;
			    while(dead<userTmp-*users){
				    end=wait(&status);
				    if(end==-1){
				       perror("\nError al esperar al hijo \n");
				       exit(-1);
				    }
				    dead++;
			    }			
			}
			*users=*users+1;
			userTmp=*users;
	                    
	    	}
	    }
	    if(pid!=0){
	    int r;
	    printf("\nEl servidor esta lleno se va adios :");
	    r=close(clienteId);
	    if(r<0){
	    	perror("Error al cerrar cliente");
	    	exit(-1);
	    }else{
	    	printf("Cerro cliente");
	    }
	    r=close(serverId);
	    if(r<0){
	    	perror("Error al cerrar servidor");
	    	exit(-1);
	    }else{
	    	printf("Cerro servidor");
	    }
	    while (*users > 0){						//Cuando el servidor no acepta mas clientes
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
			    printf("\nUsuarios %i",*users);
		    }
	    }
	    
	    }else{
	    
	    exit(0); //si el hijo llega aca no tiene nada que hacer
	    }
	    
}

int isFull(){    
    if(*users<=BACKLOG){    		
          return 1;		//para parar el while
    }else{
          return 0; 	//para continuar el while
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

void atenderCliente(int clientId, char *ipAddr){     //esta funcion atiende al cliente
	int vivo=0;
	int r,opc=0;
	do{	
		r= recv(clientId,&opc,sizeof(int),0);
		if(r<0){
			perror("\n Error al recibir solicitud");
			exit(-1);
		}
		switch(opc){
			case 1 :
				ingresar(clientId,ipAddr);
				break;

			case 2 : 
				leer(clientId,ipAddr);
				break;
			case 3 : 
				borrar(clientId,ipAddr);
				break;
			case 4 :
				buscar(clientId,ipAddr);
				break;
			case 5 : *users = *users-1;
				printf("salio uno %i",*users);
				break;
			case 0 : *users = *users-1;
			       	printf("\nsalio uno %i",*users);
				perror("\nEl usuario se desconecto repentinamente");
				exit(-1);
				break;
			default : perror ("\nOpcion invalida");
				  printf("\n%i",opc);
				  opc=0;
				  vivo++;
				  break;
		}
		if(vivo>20){
			*users = *users-1;
		       	printf("\nsalio uno %i",*users);
			perror("\nEl usuario se desconecto repentinamente");
			exit(-1);
		}
	}while(opc!=5);
}

void ingresar(int clientId, char *ipAddr){
	struct dogType *ingreso;
	ingreso= malloc(sizeof(struct dogType));
	FILE *file;
	int r;
/*	printf("\n----------Ingresar Registro----------\n");*/
	recvPerro(ingreso,clientId);
	do{
	printf("\n %i",*writeFile);
	if(*writeFile==0){
		file=openFileA("dataDogs.dat");        //Abrir el archivo para escribir
		printf("\nDespues de abrir\n");
		int data = fwrite(ingreso,sizeof(struct dogType),1,file);		
		if(data<=0){
			perror("Error de escritura");
		}else{
			printf("\nDespues de escribir\n");
		}
		fileEnd(file);
		printf("Despues de cerrar");
		writeLog(1,ingreso,ipAddr);
		free(ingreso);
	}else{
		printf("Esperando para escribir");
	}
	}while(*writeFile==1);

}


void leer(int clientId, char *ipAddr){
	int numeroRegistros = 0,r;
	struct dogType *lectura;
	long tamano=sizeof(struct dogType);
	lectura = malloc(tamano);
	FILE *file;
	numeroRegistros = numRegs();
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
	while(*writeFile);
	file=openFileR();
	if((! numeroRegistros == 0 )&& (fseek(file,opcion*tamano,SEEK_SET)==0)){
		fread(lectura,sizeof(struct dogType),1,file);
		sendPerro(lectura,clientId);
	}else{
		printf("\nNo se encontro\n");
	}
	fileEnd(file);
	writeLog(2,lectura,ipAddr);
	free(lectura);
}
void buscar(int clientId,char *ipAddr){
	int numeroRegistros = 0, numRegistro, encontrados=0,r,tam,siguiente=0;//siguiente -1 si termino 0 si debe continuar esperando 1 si encontro algo;
	struct dogType *busqueda;
	long tamano=sizeof(struct dogType),tamArchivo;
	char nameIn[32]=" ",nameTmp[32]=" ",opcion[32]=" ";
	FILE *file;
	busqueda = malloc(tamano);
	numeroRegistros = numRegs();
	r = send(clientId,&numeroRegistros, sizeof(int), 0);
	if(r<0){
		perror("error al mandar la cantidad de registros");
		exit(-1);
	}
	r=recv(clientId,&tam,sizeof(int),0);
	if(r<0){
		perror("error al recibir tamano de la palabra");
		exit(-1);
	}
	r=recv(clientId,opcion,tam,0);
	if(r<0){
		perror("error al recibir la palabra");
		exit(-1);
	}
	printf("\n%s %i",opcion,tam);
	while(*writeFile);
	file=openFileR();
	fseek(file, 0, SEEK_END);
	tamArchivo=ftell(file);
	for(numRegistro=0;numRegistro*tamano<tamArchivo; numRegistro++){
		fseek(file,numRegistro*tamano,SEEK_SET);
		fread(busqueda,tamano,1,file);
		strcpy(nameIn,opcion);
		strcpy(nameTmp,busqueda->nombre);
		minToMay(nameIn);
		minToMay(nameTmp);
		if(strcmp(nameIn,nameTmp) == 0){
			siguiente=1;
		}else{
			siguiente=0;
		}
		r=send(clientId,&siguiente,sizeof(int),0);
		if(r<0){
			perror("Error al enviar siguiente");
			exit(-1);		
		}
		if(siguiente==1){
		sendPerro(busqueda,clientId);
		encontrados++;
		}
	
	}
	siguiente=-1;
	r=send(clientId,&siguiente,sizeof(int),0);
	if(r<0){
		perror("Error al enviar el ultimo siguiente");
		exit(-1);
	}
	r=send(clientId,&encontrados,sizeof(int),0);
	if(r<0){
		perror("Error al enviar el total de encontrados");
		exit(-1);
	}
	fileEnd(file);
	writeLog(4,opcion,ipAddr);
	free(busqueda);
}
void imprimirPerro(void *ap){
	struct dogType *impreso;
	impreso = ap;
	printf("\n Nombre: %s",impreso->nombre);
	printf("\n Edad: %i",impreso->edad);
	printf("\n Raza: %s",impreso->raza);
	printf("\n Estatura: %i",impreso->estatura);
	printf("\n Peso: %3.2f",impreso->peso);
	printf("\n sexo: %c",impreso->sexo);
	printf("\n");	
}

void borrar(int clientId,char *ipAddr){
	
	int found = 0,r,opcion=0;
	int numeroRegistros = 0;
	long tamano=sizeof(struct dogType);
	struct dogType *borrado;
	FILE *file,*newfile;
	do{	
		numeroRegistros = numRegs();
		borrado = malloc(tamano);
		r = send(clientId,&numeroRegistros, sizeof(int), 0); //encia el numero de registros actuales.
		if(r<0){
			perror("Error para enviar el numero de registros");
			exit(-1);
		}
		r = recv(clientId,&opcion,sizeof(int),0); //Recibe el numero del registro que se desea eliminar
		if(r<0){
			perror("Error para recibir la opcion");
			exit(-1);
		}
		while(*writeFile); //espera mientras desocupan el archivo
		file=openFileR();
		if(fseek(file,opcion*tamano,SEEK_SET)==0){ //confirma que aun exista.	
			found=1;	
		}else{
			fileEnd(file);
		}
		r = send(clientId,&found, sizeof(int), 0); //confirma la existencia del registro aun
		if(r<0){
			perror("Error al confirmar la existencia");
			exit(-1);
		}
	}while(!found);
	fread(borrado,sizeof(struct dogType),1,file);	
	sendPerro(borrado,clientId);//envia el perro que se borrara
	rewind(file);	
	newfile = fopen("temp.dat","w+");
	printf(".-----------------Borrar-----------%i\n",opcion );
	while (fread(borrado,sizeof(struct dogType),1,file) != 0) {
		if (opcion == ftell(file)/tamano-1) {
			printf("Perro Borrado.\n\n");
			
		} else {
			fwrite(borrado, sizeof(struct dogType), 1, newfile);
		}
	}

	fclose(file);
	fclose(newfile);

	remove("dataDogs.dat");
	rename("temp.dat", "dataDogs.dat");
	writeLog(3,borrado,ipAddr);
	free(borrado);
	*writeFile=0;

}


FILE* openFileA(char *nombre){  //metodo para abrir los archivos append
	printf("\nabrio archivo A\n");
	FILE *file;
	*writeFile=1;
	file= fopen(nombre,"a+");
	if(file==NULL){
		perror ("\nError al abrir el archivo para escritura");
		exit(-1);
	}else{
		printf("abriendo archivo");
		return file;
	}
}

FILE* openFileR(){  //metodo para abrir los archivos read
	printf("\nabrio archivo R\n");
	FILE *file;
	*writeFile=1;
	file = fopen("dataDogs.dat","r"); 
	if(file==NULL){
		perror("Archivo con los datos no existe, primero haga insertar");
	}else{
		return file;
	}
}

void fileEnd(FILE  *file){   //metodo para cerrar los archivos
	printf("Entro a cerrar");
	int r=fclose(file);
	if(r==0){
		printf("Cerro el archivo");
	}else{		
		perror("\nError al cerrar el archivo");
		exit(-1);
	}
	*writeFile=0;
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
  struct dogType *enviado;
  enviado = ap;
  int r, tam;
  tam=tamano(enviado->nombre);
  r=send(clientId,&tam,sizeof(int),0);
  if(r<0){
  perror("error en send tam nombre");
  exit(-1);
  }
  r=send(clientId,enviado->nombre,tam,0);
  if(r<0){
    perror("error en send nombre");
    exit(-1);
  }
  r=send(clientId,&enviado->edad,sizeof(int),0);
  if(r<0){
    perror("error en send edad");
    exit(-1);
  }
  tam=tamano(enviado->raza);
  r=send(clientId,&tam,sizeof(int),0);
  if(r<0){
    perror("error en send tam raza");
    exit(-1);
  }
  r=send(clientId,enviado->raza,tam,0);
  if(r<0){
     perror("error en send raza");
     exit(-1);
  }
  r=send(clientId,&enviado->estatura,sizeof(int),0);
  if(r<0){
    perror("error en send estatura");
    exit(-1);
  }
  r=send(clientId,&enviado->peso,sizeof(float),0);
  if(r<0){
     perror("error en send peso");
     exit(-1);
  }
  r=send(clientId,&enviado->sexo,sizeof(char),0);
  if(r<0){
    perror("error en send sexo");
    exit(-1);
  }
}
int  tamano(char *palabra){
	int i=0;
	while(palabra[i]!='\0')
	i++;
	return i;
}
void minToMay(char *string)
{
	int i=0;
	int desp='a'-'A';
	for (i=0;string[i]!='\0';++i)
	{
		if(string[i]>='a'&&string[i]<='z')
		{
			string[i]=string[i]-desp;
		}
	}
}
int numRegs(){
	FILE *file;
	int numeroRegistros = 0;
	long tamano=sizeof(struct dogType);
	while(*writeFile);
	file=openFileR();                         //Abre el archivo
	fseek(file, 0, SEEK_END);
	numeroRegistros = ftell(file)/tamano;
	rewind(file);
	fileEnd(file);
	return numeroRegistros;		
}
void writeLog(int opcion,void *opcional,char *ipAddr){
	int r;
	char *accion, *palabra;
	time_t curtime;
	curtime=time(NULL);
	FILE *doglog;
	struct dogType *impreso;
	switch(opcion){
		case 1: accion="insercion";impreso=opcional;break;
		case 2: accion="lectura";impreso=opcional;break;
		case 3: accion="borrado";impreso=opcional;break;
		case 4: accion="busqueda";palabra=opcional;break;
		default: printf("Error en log");
	}
	doglog=openFileA("serverDogs.log");
	r=fprintf(doglog,"fecha [ %s ] Cliente [ %s ] [ %s ] [",ctime(&curtime),ipAddr,accion);
	if(r<=0){
		perror("Error al escribir primera parte del log");
		exit(-1);
	}
	if(opcion == 1 || opcion == 2 || opcion == 3){
	r=fprintf(doglog," %s , %i , %s , %i , %3.2f , %c ] \n",impreso->nombre,impreso->edad,impreso->raza,impreso->estatura,impreso->peso,impreso->sexo);
	}else
		if(opcion == 4){
			r=fprintf(doglog," %s ] \n",palabra);
		}
	if(r<=0){
		perror("Error al escribir segunda parte del log");
		exit(-1);
	}
	fileEnd(doglog);
}

