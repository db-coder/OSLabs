#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

size_t getFileSize(const char* filename){
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

struct timeval tv1,tv2;
double calc_time=0;

int main(int argc, char *argv[]){
	int fd;
	int * some;
	char * pregion;
	char pregion1[1024];
	string a;
	// cin>>a;
	char b[50];
	
	size_t size = getFileSize("files/foo0.txt");
	if(strcmp(argv[1],"map")==0){
		for(int i=0;i<25;i++){

			sprintf(b,"files/foo%d.txt",i);
			if((fd = open(b, O_RDWR)) <0 ){
				*some = fd;
				perror("failed on open");
		 		return -1;
			}
			
			int k = 0;
			off_t offset = 0;
			
			
			pregion=(char *)mmap(NULL, size, PROT_WRITE,MAP_SHARED,fd, 0);
			if (pregion==(caddr_t)-1){
				perror("mmap failed");
			 	return -1;
			}

			gettimeofday(&tv1,0);
			for(int i=0;i<size;i++){
				pregion[i] = '1';
			}

			if (msync(pregion,size, MS_SYNC) == -1)
			{
			   perror("Could not sync the file to disk");
			}
			gettimeofday(&tv2,0);
			calc_time += (tv2.tv_usec-tv1.tv_usec)+1000000*(tv2.tv_sec-tv1.tv_sec);
			munmap(pregion, size);
			
			
			close(fd);
		
			//read(fd,pregion1, 1024);
			
			//read(fd,pregion1, 1024);
		}
	}
	else {
		for(int i=0;i<25;i++){

			sprintf(b,"files/foo%d.txt",i);
			//cout<<b<<endl;
			size_t size = getFileSize(b);
			// FILE *fp;
			// fp = fopen(b,"wb");
			int fd = open(b,O_RDWR);
			
			if(fd <= 0){
				perror("ERROR reading the file");
			}
			int nread = 0;
			char buffer[512];
			strcpy(buffer,b);
			int k = 0;
			//cout<<buffer<<endl;
			gettimeofday(&tv1,0);
			while(512*k<size){
				write(fd,buffer,512);
				// fwrite (buffer , sizeof(char), sizeof(buffer), fp);
				k++;
			}
			  gettimeofday(&tv2,0);
			calc_time += (tv2.tv_usec-tv1.tv_usec)+1000000*(tv2.tv_sec-tv1.tv_sec);
			
			//read(1,some,1024);
			//read(fd,pregion1, 1024);
			close(fd);
		}
	}
	
	
	cout << double(250000000/calc_time) << endl;	
	 	
}
