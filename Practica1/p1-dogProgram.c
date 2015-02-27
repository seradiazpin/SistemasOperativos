/*-----------------------------------------------
Sistemas Operativos primera practica

Autores: Sergio Alejandro Diaz Pinilla
	 Osmar Alejandro Castillo Lancheros
-----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


//Estructura dogType para almacenar los datos de los perros.
struct dogType
{
	char nombre[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
};

FILE *file;

//Declaración de los  métodos y funciones usados

void ingresar();
void leer();
void borrar();
void buscar();
void cargar();
void imprimirPerro();
long validarEntrada();
void confirmar();
void minToMay();

/*
	Función main
*/

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
		system("clear");
	}while( opcion != '5');
}

/*
	Función para abrir el archivo dataDogs.dat
	en caso de que no exista es creado.
*/

FILE *  abrir (){  
		file = fopen("dataDogs.dat","a+");
		if(file == NULL){
			perror("\nError al abrir el archivo para escritura");
			return NULL;	
		}else{
			return file;
		}	
}

/*
	Método para abrir el archivo
*/
void cerrar(FILE *file){
	if(!fclose(file)==0){
		perror("\nError al cerrar el archivo");
	}
}

/*
	Método para ingresar los perros en el sistema 
	y guardarlos en el archivo dataDogs.dat.
	Usa el método cargar para recibir los datos del perro.
*/

void ingresar(){
	struct dogType *perros;
	printf("\n----------Ingresar Registro----------\n");
	
	perros = malloc(sizeof(struct dogType));
	cargar(perros);
	imprimirPerro(perros);
	file=abrir();
	int data = fwrite(perros,sizeof(struct dogType),1,file);
	if(data<=0){
		perror("Error de escritura");
	}
	cerrar(file);
	free(perros);
	confirmar();

}

/*
	Método para ingresar los perros datos un 
	nuevo perro que este siendo ingresando.
*/

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

/*
	Método para leer un registro del archivo dataDogs.dat
	Se leen los archivos de 0 - (n-1), donde n es el numero
	de registros.
	Para imprimir los datos se usa imprimirPerro().
*/

void leer(){
	
	int numeroRegistros = 0;
	struct dogType *lectura;
	long tamano=sizeof(struct dogType);
	lectura = malloc(tamano);
	file=abrir();
	fseek(file, 0, SEEK_END);
	numeroRegistros = ftell(file)/tamano;
	int opcion = 0;
	do{
		printf("\n----------Leer Registro---------- ");
		printf("\nPerros registrados: %i  Introdusca un numero entre 0 y %i",numeroRegistros,numeroRegistros-1);
		printf("\nRegistro:\t ");		
		scanf("%d",&opcion);	
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
	if((! numeroRegistros == 0 )&& (fseek(file,opcion*tamano,SEEK_SET)==0)){
		fread(lectura,sizeof(struct dogType),1,file);
		imprimirPerro(lectura);
	}else{
		printf("\nNo se encontro\n");
	}
	cerrar(file);
	free(lectura);
	confirmar();
}

/*
	Método que imprime los datos de un perro.
*/

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

/*
	Método para borrar un registro del archivo dataDogs.dat
	Se leen los archivos de 0 - (n-1), donde n es el numero
	de registros.
	Para hacerlo se crea un nuevo archivo temporal y se 
	renombra después de haber agregado los perros a excepcion
	del borrado.
	Imprime el perro borrado al finalizar.
*/

void borrar(){
	
	FILE *newfile;
	int found = 0;
	int numeroRegistros = 0;
	struct dogType perros;
	file = fopen("dataDogs.dat","r+");
	newfile = fopen("temp.dat","w+");
	long tamano=sizeof(struct dogType);
	fseek(file, 0, SEEK_END);
	numeroRegistros = ftell(file)/tamano;
	int opcion = 0;
	do{
		printf("\n----------Borrar Registro---------- ");
		printf("\nPerros registrados: %i Introdusca un numero entre 0 y %i",numeroRegistros,numeroRegistros-1);
		printf("\nRegistro que desea borrar:\t ");		
		scanf("%d",&opcion);	
		if(opcion<0 || opcion >= numeroRegistros){
			printf("Introdusca un registro correcto\n");
		}
	}while((! numeroRegistros == 0 )&& (opcion<0 || opcion >= numeroRegistros));
	rewind(file);
	while (fread(&perros,sizeof(struct dogType),1,file) != 0) {
		if (opcion == ftell(file)/tamano-1) {
			printf("Perro Borrado.\n\n");
			imprimirPerro(&perros);
			found=1;
		} else {
			fwrite(&perros, sizeof(struct dogType), 1, newfile);
		}
	}
	if (! found) {
		printf("No se encontro el registro n: %d\n\n", opcion);
	}

	fclose(file);
	fclose(newfile);

	remove("dataDogs.dat");
	rename("temp.dat", "dataDogs.dat");

	confirmar();

}

/*
	Método para buscar un perro del archivo dataDogs.dat por
	su nombre.
	No se distinguen mayúsculas de minúsculas y se retornan 
	todos los perros que coinciden.
*/

void buscar(){
	
	int numeroRegistros = 0, numRegistro, encontrados=0;
	char nameIn[32],nameTmp[32];
	struct dogType *busqueda;
	long tamano=sizeof(struct dogType), tamArchivo;
	busqueda = malloc(tamano);
	file=abrir();
	fseek(file, 0, SEEK_END);
	tamArchivo=ftell(file);
	numeroRegistros = tamArchivo/tamano;
	char opcion [32];
	printf("\n----------Buscar Registro---------- ");
	printf("\nPerros registrados: %i",numeroRegistros);
	printf("\nDigite el nombre del perro :\t ");		
	scanf(" %31[^\n]",opcion);			
	for(numRegistro=0 ; numRegistro*tamano < tamArchivo ; numRegistro++){
		fseek(file,numRegistro*tamano,SEEK_SET);
		fread(busqueda,tamano,1,file);
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
	cerrar(file);
	free(busqueda);
	confirmar();
}

/*
	Método para hacer Presione cualquier tecla para continuar...
*/

void confirmar(){
	system("read -sn 1 -p 'Presione cualquier tecla para continuar...'");
	printf("\n");
}

/*
	Método para poner todas las letras de un string en mayúsculas.
	Se usa en el método buscar().
*/

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
