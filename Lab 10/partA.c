#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
	int filed, ret;
	size_t file_len;
	char *addr;
	struct stat st;
	char buff;

	if((filed = open("files/foo0.txt",O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) < 0)
	{
		perror("Error opening file!");
		return EXIT_FAILURE;
	}
	if((ret = fstat(filed,&st)) < 0)
	{
		perror("Error in fstat!");
		return EXIT_FAILURE;
	}
	file_len = st.st_size;
	char a;
	printf("Initialized.\nPress enter to continue\n");
	scanf("%c",&a);
	if((addr = mmap(NULL,file_len,PROT_READ|PROT_WRITE,MAP_SHARED,filed,0)) == MAP_FAILED)
	{
		perror("Error in mmap");
		return EXIT_FAILURE;
	}
	printf("Memory Mapped.\nPress enter to continue\n");
	scanf("%c",&a);
	buff = addr[0];
	printf("First character: %c\n",buff);
	printf("Press enter to continue\n");
	scanf("%c",&a);
	buff = addr[10000];
	printf("Character at 10000 bytes offset: %c\n",buff);
	printf("Press enter to continue\n");
	scanf("%c",&a);
	close(filed);
	return 0;
}