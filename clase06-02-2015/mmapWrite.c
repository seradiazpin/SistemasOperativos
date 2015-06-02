#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>



#define FILE_LENGTH 0X100

int main(int argc, char* const argv[]){
	int fd, i;
	void* file_memory;
	fd = open(argv[1],O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	file_memory = mmap(0,FILE_LENGTH,PROT_WRITE, MAP_SHARED, fd,0);

	for (int i = 0; i < 100; i++)
	{
		sprintf((char*) file_memory+(i*4),"\n%03d",i);

	}
	munmap(file_memory,FILE_LENGTH);
	close(fd);
	return 0;
}