#include<stdio.h>
#include<stdlib.h>
struct datos
{
	int edad;
	char genero;
	char nombre[32];
	float estatura;
};

int main(){
	struct datos *persona;
	persona = malloc(sizeof(struct datos));
	cargar(persona);
	imprimir(persona);
	free(persona);
}
cargar(void *ap){
	struct datos *persona;
	persona = ap;
	printf("\n Edad: ");
	scanf(" %i",&persona->edad);
	printf("\n Genero M/F: ");
	scanf(" %c",&persona->genero);
	printf("\n Nombre: ");
	scanf(" %s",persona->nombre);
	printf("\n Estatura: ");
	scanf(" %f",&persona->estatura);	
	printf("\n");
}
imprimir(void *ap){
	struct datos *persona;
	persona = ap;
	printf("\n Edad: %i",persona->edad);
	printf("\n Genero: %c",persona->genero);
	printf("\n Nombre: %s",persona->nombre);
	printf("\n Estatura: %f",persona->estatura);
	printf("\n");	
}

