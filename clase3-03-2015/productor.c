#include<unistd.h>
#include<stdio.h>
#include<sys/shm.h>
#include<errno.h>
#include<stdlib.h>

#define FLAGON 0xFF;
#define FLAGOFF 0x00;

int main(){
	pid_t pid;
	key_t key = 123;
	int shmID,*flag;
	void *ap;
	struct dogType *dog;
	
	shmID = shmget(key,sizeof(float),IPC_CREAT | 0666);
	if(shmID == -1){
		perror("ERROR");
	}
	ap = shmat(shmID,0,0);
	if(ap < 0){
		perror("ERROR");
	}
	dato = ap;
}
