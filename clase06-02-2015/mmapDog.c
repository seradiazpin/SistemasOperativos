#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>



#define FILE_LENGTH 700

struct dogType{
  char nombre[32];
  int edad;
  char raza[16];
  int estatura;
  float peso;
  char sexo;
};
void imprimirPerro();

int main(int argc, char* const argv[]){
	int fd, i,dog;
	void* file_memory;
	scanf("%i",&dog);
	fd = open(argv[1],O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	file_memory = mmap(0,FILE_LENGTH,PROT_READ, MAP_SHARED, fd,0);
	struct dogType *lectura;
	lectura = malloc(sizeof(struct dogType));
	lectura = file_memory+(sizeof(struct dogType)*dog);
	imprimirPerro(lectura);
	lectura = NULL;
	munmap(file_memory,FILE_LENGTH);
	close(fd);
	free(lectura);
	return 0;
}

void imprimirPerro(void *ap){
  struct dogType *impreso;
  impreso = ap;
  printf("\n Nombre: %s",impreso->nombre);
  printf("\n Edad:  %i",impreso->edad);
  printf("\n Raza:  %s",impreso->raza);
  printf("\n Estatura: %i",impreso->estatura);
  printf("\n Peso: %3.2f",impreso->peso);
  printf("\n sexo: %c",impreso->sexo);
  printf("\n");
}