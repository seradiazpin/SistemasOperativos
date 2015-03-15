#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *file;

struct dogType
{
	char nombre[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
};

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
	printf("\n Edad: %i",perros->edad);
	printf("\n Raza: %s",perros->raza);
	printf("\n Estatura: %i",perros->estatura);
	printf("\n Peso: %3.2f",perros->peso);
	printf("\n sexo: %c",perros->sexo);
	printf("\n");	
}


int main(){
	int pidfd[2],r,N;
	pid_t pid;
	struct dogType *dog;


	r = pipe(pidfd);
	if(r == -1){
		perror("ERROR");
	}
	pid = fork();
	if (pid == -1)
	{
		perror("ERROR");
	}
	if (pid==0)
	{
		close(pidfd[1]);
		read(pidfd[0],&N,sizeof(int));
		read(pidfd[0],&dog,sizeof(struct dogType));
		for (int i = 0; i < N-1;i++)
		{
			imprimirPerro(&dog);
			read(pidfd[0],&dog,sizeof(struct dogType));
		}
		close(pidfd[0]);
	}else{
		close(pidfd[0]);
		printf("\n Numero de ingreso: ");
		scanf(" %i",&N);
		write(pidfd[1],&N,sizeof(int));
		for (int i = 0; i < N; ++i)
		{
			cargar(&dog);
			write(pidfd[1],&dog,sizeof(struct dogType));
		}
		close(pidfd[1]);
	}


}