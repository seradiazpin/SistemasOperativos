# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>

#define PORT 9510

struct dogType{
  char nombre[32];
  int edad;
  char raza[16];
  int estatura;
  float peso;
  char sexo;
};

void menu();
void cargar();
void imprimirPerro();
int conectar();
void confirmar();
void ingresar();
void recvPerro();
void sendPerro();
void leer();
void buscar();
void borrar();

int main( int argc,char *argv[]){
  if(argv[1]==NULL){
  	perror("Error de sintax intente e.g. ./p2-dogClient 127.0.0.1");
  	exit(-1);
  }
  int clienteId;
  struct dogType *dog;
  clienteId=conectar(argv[1]);		// crea y conecta el socket
  menu(clienteId);
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

void menu(int clientId){
  int r,i;
  char opcion = ' ';
  do{
      printf("\tMASCOTAS\n");
      printf("-------------------------\n");
      printf("|1)Ingresar Registro\t|\n");
      printf("|2)Leer Registro\t|\n");
      printf("|3)Borrar Registro\t|\n");
      printf("|4)Buscar Registro\t|\n");
      printf("|5)Salir\t\t|\n");
      printf("-------------------------\n");
      printf("Opcion:\t ");
      opcion = getchar();
      switch(opcion){
        case('1'):
		i=1;
		r=send(clientId,&i,sizeof(int),0);
		if(r<0){
		perror("Error en send: ");
		exit(-1);
		}
		ingresar(clientId);
		break;

        case('2'):
		i=2;
		r=send(clientId,&i,sizeof(int),0);
		if(r<0){
		perror("Error en send: ");
		exit(-1);
		}
		leer(clientId);
		break;
        case('3'):
		i=3;
		r=send(clientId,&i,sizeof(int),0);
		if(r<0){
		perror("Error en send: ");
		exit(-1);
		}
		borrar(clientId);
		break;
        case('4'):
		i=4;
		r=send(clientId,&i,sizeof(int),0);
		if(r<0){
		perror("Error en send: ");
		exit(-1);
		}
		buscar(clientId);
		break;	
        case('5'):
		i=5;
		r=send(clientId,&i,sizeof(int),0);
		if(r<0){
		perror("Error en send: ");
		exit(-1);
		}
		opcion='5';
		break;        
        default:
		printf("Seleccione una opcion adecuada\n");
		opcion=' ';
		while(getchar()!='\n');
		break;
     }
     while(getchar()!='\n');
     system("clear");
  }while( opcion != '5');

}

void confirmar(){
  system("read -sn 1 -p 'Presione cualquier tecla para continuar...'");
  printf("\n");
}


void ingresar(int clientId){
  struct dogType *ingreso;
  printf("\n----------Ingresar Registro----------\n");  
  ingreso = malloc(sizeof(struct dogType));
  cargar(ingreso,clientId);
  free(ingreso);
  confirmar();
}


void leer(int clientId){
	int r;
	struct dogType *lectura;
	lectura=malloc(sizeof(struct dogType));
	int numeroRegistros = 0;
	r = recv(clientId,&numeroRegistros,sizeof(int),0);
	if(r<0){
		perror("Error al recv cantidad de registros");
		exit(-1);
	}
	int opcion = 0;
	do{
		printf("\n----------Leer Registro---------- ");
		printf("\nPerros registrados: %i  Introdusca un numero entre 0 y %i",numeroRegistros,numeroRegistros-1);
		printf("\nRegistro:\t ");   
		scanf("%d",&opcion);  
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while(( numeroRegistros != 0 )&& (opcion<0 || opcion >= numeroRegistros));
	r = send(clientId,&opcion, sizeof(int), 0);
	if(r<0){
		perror("Error al send numero de registro");
		exit(-1);
	}
	recvPerro(lectura,clientId);
	imprimirPerro(lectura);
	free(lectura);
	confirmar();
}


void borrar(int clientId){
	
	int found=0, r;
	int numeroRegistros = 0;
	struct dogType *borrado;
	do{
	r = recv(clientId,&numeroRegistros,sizeof(int),0);//recibe el numero de registros actuales a la hora de hacer la peticion
	if(r<0){
		perror("Error al recibir el numero de registros");
		exit(-1);	
	}
	int opcion = 0;	
	do{
		printf("\n----------Borrar Registro---------- ");
		printf("\nPerros registrados: %i Introdusca un numero entre 0 y %i",numeroRegistros,numeroRegistros-1);
		printf("\nRegistro que desea borrar:\t ");		
		scanf("%d",&opcion);	
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
	r = send(clientId,&opcion, sizeof(int), 0); //envia el numero de registro que se desea borrar
	if(r<0){
		perror("Error al enviar la opcion deseada");
		exit(-1);
	}
	r = recv(clientId,&found,sizeof(int),0);
	if(r<0){
		perror("Error al recibir confirmacion");
		exit(-1);
	}
	if(!found){
		printf("Lo sentimos tendra que volver a empezar la operacion porque el registro fue movido o ya no existe");
	}
	}while(!found); // si no existe toca volver a empezar la operacion
	borrado=malloc(sizeof(struct dogType));
	recvPerro(borrado,clientId);
	printf("Registro borrado\n");
	imprimirPerro(borrado);
	free(borrado);
	confirmar();

}
void buscar(int clientId){
	int numeroRegistros=0,r,tam,siguiente=0,numRegistro=0, encontrados=0;
	char opcion [32];
	struct dogType *buscado;
	buscado=malloc(sizeof(struct dogType));
	r = recv(clientId,&numeroRegistros,sizeof(int),0);//recibe el numero de registros actuales a la hora de hacer la peticion
	if(r<0){
		perror("Error al recibir el numero de registros");
		exit(-1);	
	}
	printf("\n----------Buscar Registro---------- ");
	printf("\nPerros registrados: %i",numeroRegistros);
	printf("\nDigite el nombre del perro :\t ");		
	scanf(" %31[^\n]",opcion);
	tam=tamano(opcion);
	printf("\n%s %i",opcion,tam);
	r=send(clientId,&tam,sizeof(int),0);
	if(r<0){
		perror("error al enviar tamano de la palabra");
		exit(-1);
	}
	r=send(clientId,opcion,tam,0);
	if(r<0){
		perror("error al mandar la palabra");
		exit(-1);
	}
	while(siguiente>=0){
		r=recv(clientId,&siguiente,sizeof(int),0);
		if(r<0){
			perror("Error al recibir siguiente");
			exit(-1);
		}
		if(siguiente==1){
			recvPerro(buscado,clientId);
			printf("\n----------El numero del registro es : %i----------",numRegistro);
			imprimirPerro(buscado);
			encontrados++;		
		}
		if(siguiente!=-1)
			numRegistro++;
	}
	r=recv(clientId,&siguiente,sizeof(int),0);
	if(r<0){
		perror("Error al recibir encontrados por el servidor");
		exit(-1);
	}
	printf("\nSe encontro cliente%i servidor%i registos con ese nombre\n\n",encontrados,siguiente);
	free(buscado);
	confirmar();
}




void cargar(void *ap,int clientId){
  	struct dogType *ingreso;
	ingreso = ap;
	printf("\n Nombre: ");
	scanf( " %31[^\n]",ingreso->nombre);
	printf("\n Edad: ");
	scanf(" %d",&ingreso->edad);
	printf("\n Raza: ");
	scanf(" %15[^\n]",ingreso->raza);
	printf("\n Estatura: ");
	scanf(" %i",&ingreso->estatura);
	printf("\n Peso: ");
	scanf(" %f",&ingreso->peso);
	do{	
		printf("\n Sexo M/H: ");
		scanf(" %c",&ingreso->sexo);
		printf("\n");
	}while(!(ingreso->sexo == 'M' || ingreso->sexo == 'H'));

	sendPerro(ingreso,clientId);
}


void imprimirPerro(void *ap){
  struct dogType *impreso;
  impreso = ap;
  printf("\n Nombre: %s",impreso->nombre);
  printf("\n Edad:  %i",impreso->edad);
  printf("\n Raza:  %s",impreso->raza);
  printf("\n Estatura: %i",impreso->estatura);
  printf("\n Peso: %3.2f",impreso->peso);
  printf("\n sexo: %c",impreso->sexo);
  printf("\n");
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







