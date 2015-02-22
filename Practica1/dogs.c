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
	while(salirVar == 1){
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
				salir();
				break;
			default:
				printf("Seleccione una opcion adecuada\n");
				break;
		}
	}
}


ingresar(){
	struct perro *perros;
	printf("----------Ingresar Registro----------\n");
	FILE *fd;
	perros = malloc(sizeof(struct perro));
	cargar(perros);
	fd = fopen("dogsData.dat","w+");
	int data = fwrite(perros,sizeof(struct perro),1,fd);
	if (fd == NULL){
		perror("ERROR");
	}
	fclose(fd);
	//imprimir(persona);
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
int salir(){
	printf("Exit\n");
	exit(1);

}