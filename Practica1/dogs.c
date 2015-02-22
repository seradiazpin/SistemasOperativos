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

void abrir (FILE *file){
	file=fopen("dataDogs.dat","a+");
	if(file==NULL){
		perror("Error al abrir el archivo");
	}	
}
ingresar(){
	struct perro *perros;
	printf("\n----------Ingresar Registro----------\n");
	FILE *fd;
	perros = malloc(sizeof(struct perro));
	cargar(perros);
	fd = fopen("dogsData.dat","a+");
	int data = fwrite(perros,sizeof(struct perro),1,fd);
	if (fd == NULL){
		perror("ERROR");
	}
	fclose(fd);
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
/*No pude hacer que lo leyera normal solo lo puede leer en binario
tambien faltan las comprobaciones de entradas para que no saque error*/
leer(){
	FILE *fd;
	int numeroRegistros = 0;
	struct perro *lectura;
	lectura = malloc(sizeof(struct perro));
	fd = fopen("dogsData.dat","r+");
	fseek(fd, 0, SEEK_END);
	numeroRegistros = ftell(fd)/sizeof(struct perro);
	printf("\n----------Leer Registro---------- ");
	printf("\nPerros registrados: %i",numeroRegistros);
	printf("\nRegistro:\t ");
	int opcion = 0;
	scanf("%d",&opcion);
	if(fseek(fd,opcion,SEEK_SET)==0){
		fread(lectura,sizeof(struct perro),1,fd);
		imprimirPerro(lectura);
	}else{
		printf("Introdusca un registro correcto\n");
	}
	fclose(fd);
	free(lectura);
	
}
imprimirPerro(void *ap){
	struct perro *perros;
	perros = ap;
	printf("\n Nombre: %s",perros->nombre);
	printf("\n Edad: %i",perros->edad);
	printf("\n Raza: %s",perros->raza);
	printf("\n Peso: %f",perros->peso);
	printf("\n sexo: %c",perros->sexo);
	printf("\n");	
}

borrar(){
	printf("Borrar\n");
}
buscar(){
	printf("Buscar\n");
}
