/*-----------------------------------------------
Sistemas Operativos primera practica

Autores: Sergio Alejandro Diaz Pinilla
	 Osmar Alejandro Castillo Lancheros
-----------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

struct perro
{
	char nombre[32];
	int edad;
	char raza[16];
	float estatura;
	float peso;
	char sexo;
};

int main(){
	int opcion = 0;
	do{
		opcion=0;
		printf("\tMASCOTAS\n");
		printf("-------------------------\n");
		printf("|1)Ingresar Registro\t|\n");
		printf("|2)Leer Registro\t|\n");
		printf("|3)Borrar Registro\t|\n");
		printf("|4)Buscar Registro\t|\n");
		printf("|5)Salir\t\t|\n");
		printf("-------------------------\n");
		printf("Opcion:\t ");		
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
				opcion=5;
				break;
			default:
				printf("Seleccione una opcion adecuada\n");
				break;
		}
	}while( opcion != 5);
}

FILE *  abrir (){  
		FILE *fi;
		fi=fopen("dataDogs.dat","a+");
		if(fi == NULL){
			perror("\nError al abrir el archivo para escritura");
			return NULL;	
		}else{
			return fi;
		}	
}
void cerrar(FILE *file){
	if(!fclose(file)==0){
		perror("\nError al cerrar el archivo");
	}
}

ingresar(){
	struct perro *perros;
	printf("\n----------Ingresar Registro----------\n");
	FILE *fd;
	perros = malloc(sizeof(struct perro));
	cargar(perros);
	imprimirPerro(perros);
	fd=abrir();
	int data = fwrite(perros,sizeof(struct perro),1,fd);
	if(data<=0){
		perror("Error de escritura");
	}
	cerrar(fd);
	free(perros);

}
cargar(void *ap){
	struct perro *ingreso;
	ingreso = ap;
	printf("\n Nombre: ");
	scanf( " %31[^\n]",ingreso->nombre);
	printf("\n Edad: ");
	scanf(" %d",&ingreso->edad);
	printf("\n Raza: ");
	scanf(" %15[^\n]",ingreso->raza);
	printf("\n Estatura: ");
	scanf(" %f",&ingreso->estatura);
	printf("\n Peso: ");
	scanf(" %f",&ingreso->peso);
	do{	
		printf("\n Sexo M/H: ");
		scanf(" %c",&ingreso->sexo);
		printf("\n");
	}while(!(ingreso->sexo == 'M' || ingreso->sexo == 'H'));

}
leer(){
	FILE *fd;
	int numeroRegistros = 0;
	struct perro *lectura;
	long tamano=sizeof(struct perro);
	lectura = malloc(tamano);
	fd=abrir();
	fseek(fd, 0, SEEK_END);
	numeroRegistros = ftell(fd)/tamano;
	int opcion = 0;
	do{
		printf("\n----------Leer Registro---------- ");
		printf("\nPerros registrados: %i",numeroRegistros);
		printf("\nRegistro:\t ");		
		scanf("%d",&opcion);	
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
	if((! numeroRegistros == 0 )&& (fseek(fd,opcion*tamano,SEEK_SET)==0)){
		fread(lectura,sizeof(struct perro),1,fd);
		imprimirPerro(lectura);
	}else{
		printf("\nNo se encontro\n");
	}
	cerrar(fd);
	free(lectura);
	
}
imprimirPerro(void *ap){
	struct perro *perros;
	perros = ap;
	printf("\n Nombre: %s",perros->nombre);
	printf("\n Edad: %i",perros->edad);
	printf("\n Raza: %s",perros->raza);
	printf("\n Estatura: %f",perros->estatura);
	printf("\n Peso: %f",perros->peso);
	printf("\n sexo: %c",perros->sexo);
	printf("\n");	
}

borrar(){
	FILE *fd;
	FILE *newfd;
	int found = 0;
	int numeroRegistros = 0;
	struct perro perros;
	fd = fopen("dataDogs.dat","r+");
	newfd = fopen("temp.dat","w+");
	long tamano=sizeof(struct perro);
	fseek(fd, 0, SEEK_END);
	numeroRegistros = ftell(fd)/tamano;
	int opcion = 0;
	do{
		printf("\n----------Borrar Registro---------- ");
		printf("\nPerros registrados: %i",numeroRegistros);
		printf("\nRegistro que desea borrar:\t ");		
		scanf("%d",&opcion);	
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
	rewind(fd);
	while (fread(&perros,sizeof(struct perro),1,fd) != 0) {
		if (opcion == ftell(fd)/tamano-1) {
			printf("Perro Borrado.\n\n");
			imprimirPerro(&perros);
			found=1;
		} else {
			fwrite(&perros, sizeof(struct perro), 1, newfd);
		}
	}
	if (! found) {
		printf("No se encontro el registro n: %d\n\n", opcion);
	}

	fclose(fd);
	fclose(newfd);

	remove("dataDogs.dat");
	rename("temp.dat", "dataDogs.dat");

}

buscar(){
	FILE *fd;
	int numeroRegistros = 0, numRegistro, encontrados=0;
	struct perro *busqueda;
	long tamano=sizeof(struct perro), tamArchivo;
	busqueda = malloc(tamano);
	fd=abrir();
	fseek(fd, 0, SEEK_END);
	tamArchivo=ftell(fd);
	numeroRegistros = tamArchivo/tamano;
	char opcion [32];
	printf("\n----------Buscar Registro---------- ");
	printf("\nPerros registrados: %i",numeroRegistros);
	printf("\nDigite el nombre del perro :\t ");		
	scanf(" %31[^\n]",&opcion);			
	for(numRegistro=0 ; numRegistro*tamano < tamArchivo ; numRegistro++){
		fseek(fd,numRegistro*tamano,SEEK_SET);
		fread(busqueda,tamano,1,fd);
		if(strcmp(opcion,busqueda->nombre) == 0){
			printf("\n----------El numero del registro es : %i----------",numRegistro);
			imprimirPerro(busqueda);
			printf("\n--------------------------------------------------\n");
			encontrados=encontrados+1;
		}		
	}
	printf("\nSe encontro %i registos con ese nombre\n\n",encontrados);
	cerrar(fd);
	free(busqueda);
}
