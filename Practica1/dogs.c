/*-----------------------------------------------
Sistemas Operativos primera practica

Autores: Sergio Alejandro Diaz Pinilla
	 Osmar Alejandro Castillo Lancheros
-----------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
struct perro
{
	char nombre[32];
	int edad;
	char raza[16];
	float peso;
	char sexo;
};

int main(){
	int salirVar = 1;
	FILE *file;
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
		int opcion = 0;
		scanf("%d",&opcion);
		switch(opcion){
			case(1):
				ingresar();
				break;
			case(2):
				leer();
				break;
			case(3):
				borrar();
				break;
			case(4):
				buscar();
				break;
			case(5):
				/* Aca no seria mejor un simple return 0*/
				return 0;
				break;
			default:
				printf("Seleccione una opcion adecuada\n");
				break;
		}
	}while(salirVar==1);
}

void abrir (FILE *file, bool AW){  //Recibe un apuntador al cual le asigna el archivo, el bool es para saber si se va a agregar o a leer TRUE= leer FALSE=agregar
	if(AW){
		file=fopen("dataDogs.dat","w");
		if(file==NULL){
				perror("Error al abrir el archivo de lectura");
		}	
	}else{
		file=fopen("dataDogs.dat","a+");
		if(file==NULL){
				perror("Error al abrir el archivo para escritura");
		}
	}	
}
void cerrar(FILE *file){
	if(fclose(fd)=!0){
		perror("Error al cerrar el archivo");
	}
}

ingresar(){
	struct perro *perros;
	printf("----------Ingresar Registro----------\n");
	FILE *fd;
	perros = malloc(sizeof(struct perro));
	cargar(perros);
	abrir(fd,FALSE)//fd = fopen("dogsData.dat","w+");
	int data = fwrite(perros,sizeof(struct perro),1,fd);
	cerrar(fd);
	free(perros);

}
cargar(void *ap){
	struct perro *ingreso;
	ingreso = ap;
	printf("\n Nombre: ");
	scanf(" %s",ingreso->nombre);
	printf("\n Edad: ");
	scanf(" %i",&ingreso->edad);
	printf("\n Raza: ");
	scanf(" %s",ingreso->raza);
	printf("\n Peso: ");
	scanf(" %f",&ingreso->peso);	
	printf("\n Sexo M/H: ");
	scanf(" %c",&ingreso->sexo);
	printf("\n");

}

leer(){
	printf("Leer\n");
}
borrar(){
	printf("Borrar\n");
}
buscar(){
	printf("Buscar\n");
}
