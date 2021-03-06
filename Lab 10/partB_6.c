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
	while(i<3)
	{
		char fname[50] = "files/foo";
		char num[10];
		sprintf(num,"%d",i);
		strcat(fname,num);
		strcat(fname, ".txt");
		
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
		strcpy(buff,fname);
		int j=0;
		struct timeval t1;
		gettimeofday(&t1,NULL);

		while((512*j<file_len) && (write(filed,buff,512) != 0)) j++;

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