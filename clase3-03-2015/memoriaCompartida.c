#include<unistd.h>
#include<stdio.h>
#include<sys/shm.h>
#include<errno.h>
#include<stdlib.h>
int main(){
	pid_t pid;
	key_t key = 123;
	int shmID,status;
	void *ap;
	float *dato;
	
	shmID = shmget(key,sizeof(float),IPC_CREAT | 0666);
	if(shmID == -1){
		perror("ERROR");
	}
	ap = shmat(shmID,0,0);
	if(ap < 0){
		perror("ERROR");
	}
	dato = ap;
	pid = fork();
	if(pid == -1){
		perror("ERROR");
	}
	if(pid == 0){
		*dato = 3.14;	
	}else{
		pid = wait(&status);
		printf("\n%f\n",*dato);
	}
	shmdt(ap);
	exit(1);
	
}
