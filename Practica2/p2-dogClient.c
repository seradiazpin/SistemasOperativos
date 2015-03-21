# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>




#define PORT 9510

struct dogType
{
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
void cargar(void *ap){
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
          //                      ingresar();
          		i=1;
 			r=send(clientId,&i,sizeof(int),0);
                                break;
			case('2'):
          //                     leer();
	          	i=2;
       			r=send(clientId,&i,sizeof(int),0);
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









