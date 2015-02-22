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
	int salirVar = 1;
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
				salirVar=0;
				break;
			default:
				printf("Seleccione una opcion adecuada\n");
				break;
		}
	}while(salirVar==1);
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
	/*abrir(fd);//fd = fopen("dogsData.dat","w+");	*/
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
	scanf(" %s",ingreso->nombre);
	printf("\n Edad: ");
	scanf(" %i",&ingreso->edad);
	printf("\n Raza: ");
	scanf(" %s",ingreso->raza);
	printf("\n Estatura: ");
	scanf(" %f",&ingreso->estatura);
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
	if(fseek(fd,opcion*tamano,SEEK_SET)==0){
		fread(lectura,sizeof(struct perro),1,fd);
		imprimirPerro(lectura);
	}else{
		printf("Error de lectura");
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
	printf("Borrar\n");
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
	scanf(" %s",&opcion);			
	for(numRegistro=0 ; numRegistro*tamano < tamArchivo ; numRegistro++){
		fseek(fd,numRegistro*tamano,SEEK_SET);
		fread(busqueda,tamano,1,fd);
		if(strcmp(opcion,busqueda->nombre) == 0){
			printf("\n----------El numero del registro es : %i----------",numRegistro);
			imprimirPerro(busqueda);
			printf("\n--------------------------------------------------");
			encontrados=encontrados+1;
		}		
	}
	printf("\nSe encontro %i registos con ese nombre\n",encontrados);
	cerrar(fd);
	free(busqueda);
}
