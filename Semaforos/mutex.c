#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_BUFFER 16
#define MAX_DATOS 1000

//Variablesglobales
pthread_mutex_t mutex;
pthread_cond_t vacio,lleno;

int buffer[MAX_BUFFER], numdatos;
*consumidor(){
	struct datos_tipo *datos_proceso;
	int a,i,j,p,posicion=0,dato;
	for(i=0;i<MAX_DATOS;i++){
		pthread_mutex_lock(&mutex);
		while(numdatos==0)
			pthread_cond_wait(&vacio,&mutex);
		dato=buffer[posicion];
		if(posicion==15)
			posicion=0;
		else
			posicion++;
		numdatos--;
		if(numdatos==MAX_BUFFER-1)
			pthread_cond_signal(&lleno);
		pthread_mutex_unlock(&mutex);
		printf("\nsehaconsumidoeldato:%d",dato);
		fflush(stdout);
		sleep(1);
	}
}

* productor(){
	struct datos_tipo *datos_proceso;
	int a, i, j, p, posicion = 0, dato;
	for(i=0; i<MAX_DATOS; i++){
		pthread_mutex_lock(&mutex);
		while(numdatos == MAX_BUFFER)
			pthread_cond_wait(&lleno, &mutex);
		buffer[posicion] = i;
		dato = i;
		if(posicion == 15)
			posicion = 0;
		else
			posicion ++;
		numdatos ++;
		if(numdatos == 1)
			pthread_cond_signal(&vacio);
		pthread_mutex_unlock(&mutex);
		printf("\nse ha producido el dato: %d", dato);
		fflush(stdout);
	}
	pthread_exit(0);
}

main(){
	int error;
	char *valor_devuelto;
	/* Variables para hilos*/
	pthread_t idhilo1, idhilo2, idhilo3, idhilo4;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&lleno, NULL);
	pthread_cond_init(&vacio, NULL);
	error=pthread_create(&idhilo1, NULL, (void *)productor, NULL);
	if (error != 0){
		perror ("No puedo crear hilo");
		exit (-1);
	}
	error=pthread_create(&idhilo2, NULL, (void *)consumidor, NULL);
	if (error != 0){
		perror ("No puedo crear thread");
		exit (-1);
	}
	pthread_join(idhilo2, (void **)&valor_devuelto);
	pthread_join(idhilo1, (void **)&valor_devuelto);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&lleno);
	pthread_cond_destroy(&vacio);
	return 0;
}