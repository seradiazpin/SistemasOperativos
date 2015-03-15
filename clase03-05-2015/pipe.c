#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>

int main(){

	int pipefd[2],r;
	pid_t pid;
	float dato;
	
	r = pipe(pipefd);
	if(r == -1){
		perror("ERROR");	
	}
	pid = fork();
	if(pid == -1){
		perror("ERROR");
	}
	if(pid == 0){
		close(pipefd[1]); // Close the read end
		read(pipefd[0],&dato,sizeof(float));
		printf("\n%f\n",dato);
		close(pipefd[0]);

	}else{
		close(pipefd[0]); // Close the write end
		dato = 3.14;
		write(pipefd[1],&dato,sizeof(float));
		close(pipefd[1]);
	}
	exit(0);
}
