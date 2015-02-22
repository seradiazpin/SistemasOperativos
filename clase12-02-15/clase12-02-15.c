#include<stdio.h>
//tener cuidado con los punteros  error core generado
int main (){
	char *a;
	int  *b;
	long *c;
	short *d;
	double *e;
	float *g;
	
	printf("\n char: %li",sizeof(char));
	printf("\n int: %li",sizeof(int));
	printf("\n long: %li",sizeof(long));
	printf("\n short: %li",sizeof(short));
	printf("\n double: %li",sizeof(double));
	printf("\n float: %li",sizeof(float));
	
	printf("\n char: %li",sizeof(a));
	printf("\n int: %li",sizeof(b));
	printf("\n long: %li",sizeof(c));
	printf("\n short: %li",sizeof(d));
	printf("\n double: %li",sizeof(e));
	printf("\n float: %li \n",sizeof(g));

}
