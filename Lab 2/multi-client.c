#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int sockfd, portno, n, p, total, wait, rnd;//total is total duration of the expt, sleep is sleep time, random is 1 if random, 0 if fixed
int* req;
double* time_taken;

struct sockaddr_in serv_addr;
struct hostent *server;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void *process(void *i){
    time_t tv_sec;
    tv_sec = time(NULL);
    while(1){
        //printf("%s\n", serv_addr.sin_family);
        time_t tv_sec1;
        tv_sec1 = time(NULL);
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
            error("ERROR opening socket");
        
        if ((p = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))) < 0) 
            error("ERROR connecting");
        printf("%d\n", sockfd);
        int r;
        if(rnd==1){
            srand(time(NULL));
            r = rand()*(int)i % 10000;
            if (r<0)
            {
            	r=-r;
            }
        }
        else{
            r = 10000;
        }

        req[(int)i]++;

        char num[4];
        sprintf(num, "%d", r);
        strcat(num, ".txt");
        char msg[50] = "get files/foo";
        strcat(msg, num);

        n = write(sockfd,msg,strlen(msg));
        if (n < 0) error("ERROR writing to socket");
        // printf("%s\n", msg);
        char buff[1024];
        bzero(buff,1024);
        n = read(sockfd,buff,1024);
        bzero(buff,1024);
        // printf("n%d",n);
        while(n>0)
        {
            n = read(sockfd,buff,1024);
            bzero(buff,1024);
            // printf("n%d\n",n);
            if (n < 0) 
            {
                error("ERROR reading from socket");
                break;
            }
            else if(n==0)
            {
                break;
            }     
            // printf("n%d",n);
        }
        printf("Done!\n");
        close(sockfd);
        time_t tv_sec2;
        tv_sec2 = time(NULL);
        // printf("i%d %d\n",i,req[(int)i]);
        time_taken[(int)i] += difftime(tv_sec2, tv_sec1);
        if(difftime(tv_sec2, tv_sec) > total) break;
        else sleep(wait);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    time_t total_time0;
    total_time0 = time(NULL);
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    /* create socket, get sockfd handle */

    portno = atoi(argv[2]);

    /* fill in server address in sockaddr_in datastructure */

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    //printf("%s\n", serv_addr.sin_family);
    pthread_t tid[atoi(argv[3])];

    req = calloc(atoi(argv[3]),sizeof(int));
    time_taken = calloc(atoi(argv[3]),sizeof(double));

    total = atoi(argv[4]);
    wait = atoi(argv[5]);
    if(strcmp(argv[6], "random") == 0) rnd = 1;
    else rnd = 0;
    int i;
    for(i=0; i<atoi(argv[3]); i++){
        req[i]=0;
        time_taken[i]=0;
        pthread_create(&tid[i], NULL, process, (void *) i);
    }
    for(i=0; i<atoi(argv[3]); i++){
        pthread_join(tid[i], NULL);
    }
    // pthread_exit(NULL);
    // printf("Done!\n");
    time_t total_time1;
    total_time1 = time(NULL);
    int sum=0;
    double time_taken_total=0;
    for(i=0;i<atoi(argv[3]);i++)
    {
        sum+=req[i];
        time_taken_total+=time_taken[i];
        // printf("requests: %d%d\n",i,req[i] );
    }
    printf("Throughput: %f req/s\n",(double)sum/difftime(total_time1,total_time0) );
    printf("Average Response Time: %f sec\n",time_taken_total/(double)sum );
    free(req);
    free(time_taken);
    return 0;
}
