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
	int filed, ret, n, j;
	double total_size = 0, total_time=0;
	size_t file_len;
	char *addr;
	struct stat st;
	char buff[512];
	int i = 0;
	while(i<25)
	{
		char fname[50] = "files/foo";
		char num[10];
		sprintf(num,"%d",i);
		strcat(fname,num);
		strcat(fname, ".txt");
		printf("%s\n", fname);
		if((filed = open(fname,O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) < 0)
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
		total_size += file_len;

		printf("Initialized.\n");

		// if((addr = mmap(NULL,file_len,PROT_READ|PROT_WRITE,MAP_SHARED,filed,0)) == MAP_FAILED)
		// {
		// 	perror("Error in mmap");
		// 	return EXIT_FAILURE;
		// }
		// printf("Memory Mapped.\n");

		struct timeval t1;
		gettimeofday(&t1,NULL);
		// for (j = 0; j < file_len; ++j)
		// {
		// 	buff = addr[j];
		// }
		while((n = read(filed,buff,512)) != 0) bzero(buff,512);
		struct timeval t2;
		gettimeofday(&t2,NULL);
		total_time += ((t2.tv_sec-t1.tv_sec) + 0.000001*(t2.tv_usec - t1.tv_usec));
		close(filed);
		i++;
	}
	total_size /= 1024*1024;
	printf("Throughput %f MB/s\n",(double)total_size/total_time);
	return 0;
}