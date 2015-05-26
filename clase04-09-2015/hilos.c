#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *imprimir (void *arg){
	int num;
	num = (int)arg;
	printf("Hilo numero %i\n", num);
	return NULL;
}


int main()
{
	int valores [] = {1,2,3,4,5,6,7,8,9,10};
	int hilosErr;
	pthread_t hilo [10] ;

	for (int i = 0; i < 10; ++i)
	{	

		hilosErr= pthread_create(&hilo[i],NULL,imprimir,(void *)(intptr_t)valores[i]);
		if(hilosErr!=0){
			perror("ERROR CREAR HILO");
		}
	}
	for (int i = 0; i < 10; ++i)
	{
		hilosErr = pthread_join(hilo[i],NULL);
		if(hilosErr!=0){
			perror("ERROR CERRAR HILO");
		}
	}
}

