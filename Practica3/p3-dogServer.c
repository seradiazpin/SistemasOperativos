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
# include <unistd.h>
# include <pthread.h>

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

struct hijos{
	pthread_t hilo;
	char *ipAddr;
	int ocupado,clientId; // es para saber si el hilo esta activo. 1=activo 0=termino
};
int *users,on,clientesI[BACKLOG];  //variable en memoria compartida si esta en 1 es porque estan escribiendo el archivo si esta en 0 esta libre
struct hijos clientes[BACKLOG];
pthread_mutex_t mutex;

int vacio();
int desocupar();
int isFull();
int crear();
void *crearClientes();
void *eliminarClientes();
FILE* openFileR();
FILE* openFileA();
void fileEnd();
void *atenderCliente(); 
void ingresar();
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
	int serverId,r,status,shmId,userTmp;
	key_t keyU=3232;
//	pid_t pid, end; // identificador de procesos
	int i;
	for(i=0;i<BACKLOG;i++)
		clientesI[i]=0;
	pthread_t alfa, omega;
	serverId=crear();	
	shmId=shmget(keyU,sizeof(int),0666|IPC_CREAT);//Espacio de memoria para el numero de usuarios
	if(shmId<0){
		perror("Error en shmget users");
		exit(-1);
	}
	users = (int *)shmat(shmId,0,0);
	*users = 0;
	pthread_create(&alfa,NULL,crearClientes,&serverId);
	pthread_create(&omega,NULL,eliminarClientes,NULL);
	pthread_join(omega,NULL);
	pthread_join(alfa,NULL);
	exit(0);
}
int vacio(){
	int i=0;
	while((clientesI[i])!=0 && i<BACKLOG){
		i++;
	}
	printf("number %i \n",i);
	return i;
}
int desocupar(){
	int i=0;
	if(clientesI[i]!=0)
		return -1;
	while(clientesI[i]!=0 && clientes[i].ocupado!=1 && i<BACKLOG){
		i++;
	}
	return i;
}

void *crearClientes(int *serverId){
	printf("alfa\n");
	int clienteId,num;
	struct sockaddr_in  client;
	struct hijos hilo;
	socklen_t tamano=0;
	while(isFull()){
		clienteId=accept(*serverId,(struct sockaddr *)&client,&tamano);
		if(clienteId<0)
		{
			perror("\n Error en accept: \n");
			exit(-1);
		}
		hilo.clientId=clienteId;
		hilo.ipAddr = inet_ntoa(client.sin_addr);
		hilo.ocupado=1;
		num=vacio();
		clientesI[num]=1;
		clientes[num]=hilo;
		printf("cliente numero %i \n",*users);
		if (pthread_create(&clientes[num].hilo, NULL, atenderCliente, &clientes[num]) != 0) {
		    	printf("Uh-oh!\n");
		}else{
			puts("true");
			*users=*users+1;		
		}
	}
	close(clienteId);
	if(close(*serverId)==0){
		on=0;
	}
	pthread_exit(0);
}
void *eliminarClientes(){
	printf("omega");
	int dead,i;
	while(on){
		dead=desocupar();
		if(dead>0){
		pthread_join(clientes[dead].hilo,NULL);
		puts("Elimino");
		i=dead;
		while(&clientesI[i+1]!=0 && i<BACKLOG-1){
			clientes[i]=clientes[i+1];
			clientesI[i]=clientesI[i+1];
				i++;
			}
		free(&clientes[i+1]);
		*users=*users-1;
		}		
	}
	pthread_exit(0);
}

int isFull(){    
    if(*users<=BACKLOG){    		
          return 1;		//para parar el while
    }else{
          return 0; 	//para continuar el while
    }
}

int crear(){                     //crea el socket del servidor
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
	on=1;
	return serverId;
}
 //esta funcion atiende al cliente
void *atenderCliente(void *cliente){    
	struct hijos *hilo=cliente;
	int vivo=0;
	int r,opc=0;
	do{	
		r= recv(hilo->clientId,&opc,sizeof(int),0);
		if(r<0){
			perror("\n Error al recibir solicitud");
			exit(-1);
		}
		switch(opc){
			case 1 :ingresar(cliente);
				break;

			case 2 :leer(cliente);
				break;
			case 3 :borrar(cliente);
				break;
			case 4 :buscar(cliente);
				break;
			case 5 : hilo->ocupado=0;
				break;
			case 0 : hilo->ocupado=0;
				perror("\nEl usuario se desconecto repentinamente");
				opc=5;
				break;
			default : perror ("\nOpcion invalida");
				  opc=0;
				  vivo++;
				  break;
		}
		if(vivo>20){
			perror("\nEl usuario se desconecto repentinamente");
			opc=5;
		}
	}while(opc!=5);
	pthread_exit(0);
}

void ingresar(void *cliente){    
	struct hijos *hilo=cliente;
	struct dogType *ingreso = NULL;
	ingreso= malloc(sizeof(struct dogType));
	FILE *file;
	int r;
	recvPerro(ingreso,hilo->clientId);
	file=openFileA("dataDogs.dat");        //Abrir el archivo para escribir
	int data = fwrite(ingreso,sizeof(struct dogType),1,file);		
	if(data<=0){
		perror("Error de escritura");
	}
	fileEnd(file);
	writeLog(1,ingreso,hilo->ipAddr);
	free(ingreso);	
	

}


void leer(void *cliente){    
	struct hijos *hilo=cliente;
	int numeroRegistros = 0,r,found=0;
	struct dogType *lectura = NULL;
	long tamano=sizeof(struct dogType);
	lectura = malloc(tamano);
	FILE *file;
	do{
	numeroRegistros = numRegs();
	r = send(hilo->clientId,&numeroRegistros, sizeof(int), 0);
	if(r<0){
		perror("error al mandar la cantidad de registros");
		exit(-1);
	}
	int opcion = 0;
	r = recv(hilo->clientId,&opcion,sizeof(int),0);
	if(r<0){
		perror("error al recibir el numero  del registro");
		exit(-1);
	}
	file=openFileR();
	rewind(file);
	if(( numeroRegistros > 0 )&& (fseek(file,opcion*tamano,SEEK_SET)==0) && fread(lectura,sizeof(struct dogType),1,file)==1){
		found=1;
		
	}else{
		fileEnd(file);
	}
		r = send(hilo->clientId,&found, sizeof(int), 0); //confirma la existencia del registro aun
		if(r<0){
			perror("Error al confirmar la existencia");
			exit(-1);
		}
	}while(!found && numeroRegistros>0);
	if(numeroRegistros>0){
		sendPerro(lectura,hilo->clientId);
		fileEnd(file);
		writeLog(2,lectura,hilo->ipAddr);
	}
	free(lectura);
}
void buscar(void *cliente){    
	struct hijos *hilo=cliente;
	int numeroRegistros = 0, numRegistro, encontrados=0,r,tam,siguiente=0;//siguiente -1 si termino 0 si debe continuar esperando 1 si encontro algo;
	struct dogType *busqueda;
	long tamano=sizeof(struct dogType),tamArchivo;
	char nameIn[32]=" ",nameTmp[32]=" ",opcion[32]=" ";
	FILE *file;
	busqueda = malloc(tamano);
	numeroRegistros = numRegs();
	r = send(hilo->clientId,&numeroRegistros, sizeof(int), 0);
	if(r<0){
		perror("error al mandar la cantidad de registros");
		exit(-1);
	}
	r=recv(hilo->clientId,&tam,sizeof(int),0);
	if(r<0){
		perror("error al recibir tamano de la palabra");
		exit(-1);
	}
	r=recv(hilo->clientId,opcion,tam,0);
	if(r<0){
		perror("error al recibir la palabra");
		exit(-1);
	}
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
		r=send(hilo->clientId,&siguiente,sizeof(int),0);
		if(r<0){
			perror("Error al enviar siguiente");
			exit(-1);		
		}
		if(siguiente==1){
		sendPerro(busqueda,hilo->clientId);
		encontrados++;
		}
	
	}
	siguiente=-1;
	r=send(hilo->clientId,&siguiente,sizeof(int),0);
	if(r<0){
		perror("Error al enviar el ultimo siguiente");
		exit(-1);
	}
	r=send(hilo->clientId,&encontrados,sizeof(int),0);
	if(r<0){
		perror("Error al enviar el total de encontrados");
		exit(-1);
	}
	fileEnd(file);
	writeLog(4,opcion,hilo->ipAddr);
	free(busqueda);
}

void borrar(void *cliente){    
	struct hijos *hilo=cliente;	
	int found = 0,r,opcion=0;
	int numeroRegistros = 0;
	long tamano=sizeof(struct dogType);
	struct dogType *borrado;
	FILE *file,*newfile;
	do{	
		numeroRegistros = numRegs();
		borrado = malloc(tamano);
		r = send(hilo->clientId,&numeroRegistros, sizeof(int), 0); //encia el numero de registros actuales.
		if(r<0){
			perror("Error para enviar el numero de registros");
			exit(-1);
		}
		r = recv(hilo->clientId,&opcion,sizeof(int),0); //Recibe el numero del registro que se desea eliminar
		if(r<0){
			perror("Error para recibir la opcion");
			exit(-1);
		}
		file=openFileR();
		if(fseek(file,opcion*tamano,SEEK_SET)==0 && fread(borrado,sizeof(struct dogType),1,file)==1 ){ //confirma que aun exista.	
			found=1;	
		}else{
			fileEnd(file);
		}
		r = send(hilo->clientId,&found, sizeof(int), 0); //confirma la existencia del registro aun
		if(r<0){
			perror("Error al confirmar la existencia");
			exit(-1);
		}
	}while(!found && numeroRegistros>0);
	if(numeroRegistros>0){
		fread(borrado,sizeof(struct dogType),1,file);	
		sendPerro(borrado,hilo->clientId);//envia el perro que se borrara
		rewind(file);	
		newfile = fopen("temp.dat","w+");
		while (fread(borrado,sizeof(struct dogType),1,file) != 0) {
			if ( opcion != ftell(file)/tamano-1) {
				fwrite(borrado, sizeof(struct dogType), 1, newfile);
			} 
		}

		fclose(file);
		fclose(newfile);

		remove("dataDogs.dat");
		rename("temp.dat", "dataDogs.dat");
		pthread_mutex_unlock(&mutex);
		writeLog(3,borrado,hilo->ipAddr);
	}
	free(borrado);

}


FILE* openFileA(char *nombre){  //metodo para abrir los archivos append
	FILE *file;
	pthread_mutex_lock(&mutex);
	file= fopen(nombre,"a+");
	if(file==NULL){
		perror ("\nError al abrir el archivo para escritura");
		exit(-1);
	}else{
		return file;
	}
}

FILE* openFileR(){  //metodo para abrir los archivos read
	FILE *file;
	pthread_mutex_lock(&mutex);
	file = fopen("dataDogs.dat","r"); 
	if(file==NULL){
		perror("Archivo con los datos no existe, primero haga insertar");
		exit(-1);
	}else{
		return file;
	}
}

void fileEnd(FILE  *file){   //metodo para cerrar los archivos
	if(fclose(file)){
		perror("\nError al cerrar el archivo");
		exit(-1);
	}
	pthread_mutex_unlock(&mutex);
}

void recvPerro(void *ap, int clientId){
    struct dogType *recibiendo=NULL;
    recibiendo=ap;
    int tam=sizeof(struct dogType),r=0,tamTotal=0;
    do{
    r=recv(clientId,recibiendo+tamTotal,tam-tamTotal,0);
	if(r<0){perror("Error al enviar perro");exit(-1);}
    tamTotal=tamTotal+r;
    }while(tam-tamTotal!=0);
}
void sendPerro(void *ap,int clientId){
  struct dogType *enviado;
  enviado = ap;
  int tam=sizeof(struct dogType),r=0,tamTotal=0;
    do{
    r=send(clientId,enviado+tamTotal,tam-tamTotal,0);
    	if(r<0){perror("Error al enviar perro");exit(-1);}
    tamTotal=tamTotal+r;
    }while(tam-tamTotal!=0);
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
	struct tm *local;
	curtime=time(NULL);
	local=localtime(&curtime);
	FILE *doglog;
	struct dogType *impreso;
	switch(opcion){
		case 1: accion="insercion";impreso=opcional;break;
		case 2: accion="lectura";impreso=opcional;break;
		case 3: accion="borrado";impreso=opcional;break;
		case 4: accion="busqueda";palabra=opcional;break;
		default: perror("Error en log");exit(-1);break;
	}
	doglog=openFileA("serverDogs.log");
	r=fprintf(doglog," [ fecha %i/%.2i/%.2i %.2i:%.2i:%.2i ] Cliente [ %s ] [ %s ] [",(local->tm_year+1900),(local->tm_mon+1),local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec,ipAddr,accion);
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

