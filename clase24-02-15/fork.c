#include<unistd.h>
#include<stdio.h>
#include<math.h>

int main(){
	pid_t pid;
	
	pid = fork();
	
	if(pid == -1){
		perror("error");		
	}else{
		if(pid == 0){
			calcular();
			//printf("\n soy el hijo");		
		}else{
			calcular();
			//printf("\n soy tu padre\n");		
		}	
	}
}

int calcular(){
	int i;
	double data;
	for(i = 0;i< 1000000000;i++){
		data = data + cos(i);
	}
	return 0;
}
