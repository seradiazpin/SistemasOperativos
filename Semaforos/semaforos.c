#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */

#define MAX_PROCESOS 2

sem_t *semaforo;

struct datos_tipo{
	int dato;
	int p;
};

* proceso(void *datos){
	struct datos_tipo *datos_proceso;
	datos_proceso = (struct datos_tipo *)datos; 
	int a, i, j, p;
	sem_wait(semaforo);
	a = datos_proceso -> dato;
	p = datos_proceso -> p; 
	for(i=0; i <= p; i++){ 
		printf(" %i ",a);
	}
	fflush(stdout);
	sleep(1);

	for(i=0; i<=p; i++){
		printf("- ");
	}
	fflush(stdout);
	sem_post(semaforo);
}


main(){
	int error;
	char *valor_devuelto;
	struct datos_tipo hilo1_datos, hilo2_datos, hilo3_datos,hilo4_datos;
	pthread_t idhilo1, idhilo2, idhilo3, idhilo4;
	hilo1_datos.dato = 1;
	hilo2_datos.dato = 2;
	hilo3_datos.dato = 3;
	//hilo4_datos.dato = 4;
	hilo1_datos.p = 10;
	hilo2_datos.p = 5;
	hilo3_datos.p = 10;
	//hilo4_datos.p = 5;
	semaforo= sem_open("semaforo_name", O_CREAT, 0700, MAX_PROCESOS); 
	error=pthread_create(&idhilo1, NULL, (void *)proceso, (void *)(&hilo1_datos));
	if (error != 0){
		perror ("No puedo crear hilo");
		exit (-1);
	}
	error=pthread_create(&idhilo2, NULL, (void *)proceso, (void *)(&hilo2_datos));
	if (error != 0){
		perror ("No puedo crear thread");
		exit (-1);
	}
	error=pthread_create(&idhilo3, NULL, (void *)proceso, (void *)(&hilo3_datos));
	if (error != 0){
		perror ("No puedo crear hilo");
		exit (-1);
	}
	/*
	error=pthread_create(&idhilo4, NULL, (void *)proceso, (void *)(&hilo4_datos));
	if (error != 0){
		perror ("No puedo crear hilo");
		exit (-1);
	}*/
	pthread_join(idhilo2, (void **)&valor_devuelto);
	pthread_join(idhilo1, (void **)&valor_devuelto);
	pthread_join(idhilo3, (void **)&valor_devuelto);
	//pthread_join(idhilo4, (void **)&valor_devuelto);
	sem_close(semaforo);
	sem_unlink("semaforo_name");
}



