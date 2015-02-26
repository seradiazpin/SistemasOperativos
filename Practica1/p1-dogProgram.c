/*-----------------------------------------------
Sistemas Operativos primera practica

Autores: Sergio Alejandro Diaz Pinilla
	 Osmar Alejandro Castillo Lancheros
-----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct dogType
{
	char nombre[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
};

void ingresar();
void leer();
void borrar();
void buscar();
void cargar();
void imprimirPerro();
long validarEntrada();
void confirmar();
void minToMay();

int main(){
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
	
		//scanf("%c",&opcion);	
		switch(opcion){
			case('1'):
				ingresar();
				break;
			case('2'):
				leer();
				break;
			case('3'):
				borrar();
				break;
			case('4'):
				buscar();
				break;
			case('5'):
				opcion='5';
				break;
			default:
				printf("Seleccione una opcion adecuada\n");
				opcion= ' ';
				while(getchar()!='\n');
				break;
		}
		while(getchar()!='\n');
	}while( opcion != '5');
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

void ingresar(){
	struct dogType *perros;
	printf("\n----------Ingresar Registro----------\n");
	FILE *fd;
	perros = malloc(sizeof(struct dogType));
	cargar(perros);
	imprimirPerro(perros);
	fd=abrir();
	int data = fwrite(perros,sizeof(struct dogType),1,fd);
	if(data<=0){
		perror("Error de escritura");
	}
	cerrar(fd);
	free(perros);
	confirmar();

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
void leer(){
	FILE *fd;
	int numeroRegistros = 0;
	struct dogType *lectura;
	long tamano=sizeof(struct dogType);
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
		fread(lectura,sizeof(struct dogType),1,fd);
		imprimirPerro(lectura);
	}else{
		printf("\nNo se encontro\n");
	}
	cerrar(fd);
	free(lectura);
	confirmar();
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

void borrar(){
	FILE *fd;
	FILE *newfd;
	int found = 0;
	int numeroRegistros = 0;
	struct dogType perros;
	fd = fopen("dataDogs.dat","r+");
	newfd = fopen("temp.dat","w+");
	long tamano=sizeof(struct dogType);
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
	while (fread(&perros,sizeof(struct dogType),1,fd) != 0) {
		if (opcion == ftell(fd)/tamano-1) {
			printf("Perro Borrado.\n\n");
			imprimirPerro(&perros);
			found=1;
		} else {
			fwrite(&perros, sizeof(struct dogType), 1, newfd);
		}
	}
	if (! found) {
		printf("No se encontro el registro n: %d\n\n", opcion);
	}

	fclose(fd);
	fclose(newfd);

	remove("dataDogs.dat");
	rename("temp.dat", "dataDogs.dat");

	confirmar();

}

void buscar(){
	FILE *fd;
	int numeroRegistros = 0, numRegistro, encontrados=0;
	char nameIn[32],nameTmp[32];
	struct dogType *busqueda;
	long tamano=sizeof(struct dogType), tamArchivo;
	busqueda = malloc(tamano);
	fd=abrir();
	fseek(fd, 0, SEEK_END);
	tamArchivo=ftell(fd);
	numeroRegistros = tamArchivo/tamano;
	char opcion [32];
	printf("\n----------Buscar Registro---------- ");
	printf("\nPerros registrados: %i",numeroRegistros);
	printf("\nDigite el nombre del perro :\t ");		
	scanf(" %31[^\n]",opcion);			
	for(numRegistro=0 ; numRegistro*tamano < tamArchivo ; numRegistro++){
		fseek(fd,numRegistro*tamano,SEEK_SET);
		fread(busqueda,tamano,1,fd);
		strcpy(nameIn,opcion);
		strcpy(nameTmp,busqueda->nombre);
		minToMay(nameIn);
		minToMay(nameTmp);
		if(strcmp(nameIn,nameTmp) == 0){
			printf("\n----------El numero del registro es : %i----------",numRegistro);
			imprimirPerro(busqueda);
			printf("\n--------------------------------------------------\n");
			encontrados=encontrados+1;
		}		
	}
	printf("\nSe encontro %i registos con ese nombre\n\n",encontrados);
	cerrar(fd);
	free(busqueda);
	confirmar();
}
void confirmar(){
	system("read -sn 1 -p 'Presione cualquier tecla para continuar...'");
	printf("\n");
}
void minToMay(char string[])
{
	int i=0;
	int desp='a'-'A';
	for (i=0;string[i]!='\0';++i)
	{
		if(string[i]>='a'&&string[i]<='z')
		{
			string[i]=string[i]-desp;
		}
	}
}
