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

int main( int argc,char *argv[]){

  int clienteId, r;
  char buffer[32];
  struct dogType *dog;
  clienteId=conectar(argv[1]);
  r= send(clienteId,"soy yo ",11,0);

  if(r<0){
    perror("\nError en send(): ");
    exit(-1);
  }

  r=recv(clienteId,buffer,32,0);
  buffer[r]=0;
  printf("\n Mensaje recibido: %s",buffer);
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

void confirmar(){
  system("read -sn 1 -p 'Presione cualquier tecla para continuar...'");
  printf("\n");
}


void ingresar(void *ap,int clientId, int r){
  struct dogType *perros;
  perros = ap;
  printf("\n----------Ingresar Registro----------\n");
  
  perros = malloc(sizeof(struct dogType));
  cargar(perros,clientId,r);
  free(perros);
  confirmar();
}

void cargar(void *ap,int clientId,int r){
  struct dogType *ingreso;
  ingreso = ap;
  char nombre[32];
  int edad;
  char raza[16];
  int estatura;
  float peso;
  char  sexo;

  printf("\n Nombre: ");
  scanf( " %31[^\n]",nombre);
  printf("\n Edad: ");
  scanf(" %d",&edad);
  printf("\n Raza: ");
  scanf(" %15[^\n]",raza);
  printf("\n Estatura: ");
  scanf(" %i",&estatura);
  printf("\n Peso: ");
  scanf(" %f",&peso);
  do{
    printf("\n Sexo M/H: ");
    scanf(" %c",&sexo);
    printf("\n");
  }while(!(sexo == 'M' || sexo == 'H'));

  r=send(clientId,&nombre,32,0);
  r=send(clientId,&edad,sizeof(int),0);
  r=send(clientId,&raza,16,0);
  r=send(clientId,&estatura,sizeof(int),0);
  r=send(clientId,&peso,sizeof(float),0);
  r=send(clientId,&sexo,sizeof(char),0);
}


void imprimirPerro(void *ap){
  struct dogType *perros;
  perros = ap;
  printf("\n Nombre: %s",perros->nombre);
  printf("\n Edad:  %i",perros->edad);
  printf("\n Raza:  %s",perros->raza);
  printf("\n Estatura: %i",perros->estatura);
  printf("\n Peso: %3.2f",perros->peso);
  printf("\n sexo: %c",perros->sexo);
  printf("\n");
}



void leer(int clientId,int r){
  struct dogType *lectura;
  char nombre[32];
 
  int numeroRegistros = 0;
  long tamano=sizeof(struct dogType);
  lectura = malloc(tamano);
  r = recv(clientId,&numeroRegistros,sizeof(int),0);
  int opcion = 0;
  do{
    printf("\n----------Leer Registro---------- ");
    printf("\nPerros registrados: %i  Introdusca un numero entre 0 y %i",numeroRegistros,numeroRegistros-1);
    printf("\nRegistro:\t ");   
    scanf("%d",&opcion);  
    if(opcion<0 || opcion >= numeroRegistros){
      printf("Introdusca un registro correcto\n");
    }
    r = send(clientId,&opcion, sizeof(int), 0);
  }while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
    int tamNom ;
    r = recv(clientId,&tamNom,sizeof(unsigned long),0);
    r = recv(clientId,lectura->nombre,32,0);
    r = recv(clientId,&lectura->edad,sizeof(int),0);
    r = recv(clientId,lectura->raza,16,0);
    r = recv(clientId,&lectura->estatura,sizeof(int),0);
    r = recv(clientId,&lectura->peso,sizeof(float),0);
    r = recv(clientId,&lectura->sexo,sizeof(char),0);
    imprimirPerro(lectura);
  free(lectura);
  confirmar();
}


void menu(int clientId){
  int r,i;
  struct dogType perros;
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
                //                      ingresar();
        i=1;
        r=send(clientId,&i,sizeof(int),0);
        ingresar(&perros,clientId,r);
        break;

        case('2'):
                //                     leer();
        i=2;

        r=send(clientId,&i,sizeof(int),0);
        leer(clientId,r);
        break;
        case('3'):
                //                      borrar();
        i=3;
        r=send(clientId,&i,sizeof(int),0);
        break;
        case('4'):
                //                      buscar();
        i=4;
        r=send(clientId,&i,sizeof(int),0);
        break;

        case('5'):
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
     if(r<0){
      perror("Error en send: ");
      exit(-1);
    }
  }while( opcion != '5');

}
void recvPerro(void *ap, int clientId){
    struct dogType *lectura;
    lectura = ap;
    int r, tam;
    r= recv(clientId,&tam,sizeof(tam),0);
    if(r<0){
      perror("Error recv tamano nombre");
      exit(-1);
    }
    r = recv(clientId,lectura->nombre,tam,0);
    if(r<0){
  	perror("Error recv nombre");
  	exit(-1);
    }
    r = recv(clientId,&lectura->edad,sizeof(int),0);
    if(r<0){
          perror("Error recv edad");
          exit(-1);
    }
    r= recv(clientId,&tam,sizeof(int),0);
    if(r<0){
        perror("Error recv tam raza");
        exit(-1);
    }
    r = recv(clientId,lectura->raza,tam,0);
    if(r<0){
         perror("Error recv raza");
         exit(-1);
    }
    r = recv(clientId,&lectura->estatura,sizeof(int),0);
    if(r<0){
        perror("Error recv estatura");
        exit(-1);
    }
    r = recv(clientId,&lectura->peso,sizeof(float),0);
    if(r<0){
           perror("Error recv peso");
           exit(-1);
    }
    r = recv(clientId,&lectura->sexo,sizeof(char),0);
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








