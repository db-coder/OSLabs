#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

int N = 10;
int K = 10000;
int count;

void *process(){
    int i;
    for(i=0; i<K; i++){
    	count++;
    }
}

int main(int argc, char *argv[])
{
	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
    
    pthread_t tid[N];
    count = 0;
    
    int i;
    for(i=0; i<N; i++){
        pthread_create(&tid[i], NULL, process, NULL);
    }
    for(i=0; i<N; i++){
        pthread_join(tid[i], NULL);
    }
    // pthread_exit(NULL);
    printf("Done!\n");
    gettimeofday(&t2, NULL);
    int final = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec);
    printf("Count is %d\n", count);
    printf("Time taken is %d microseconds\n", final);
    return 0;
}