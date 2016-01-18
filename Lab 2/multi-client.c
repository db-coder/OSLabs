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

struct sockaddr_in serv_addr;
struct hostent *server;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void *process(){
    time_t tv_sec;
    while(1){
        //printf("%s\n", serv_addr.sin_family);
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
            r = rand() % 10000;
        }
        else{
            r = 10000;
        }
        char num[4];
        sprintf(num, "%d", r);
        strcat(num, ".txt");
        char msg[50] = "get files/foo";
        strcat(msg, num);
        printf("asdf\n");
        n = write(sockfd,msg,strlen(msg));
        if (n < 0) error("ERROR writing to socket");
        printf("%s\n", msg);
        char buff[512];
        while(1)
        {
            n = read(sockfd,buff,512);
            if (n <= 0) 
            {
                //error("ERROR reading from socket");
                break;
            }     
            printf("%s\n",buff);
        }
        close(sockfd);
        time_t tv_sec2;
        if(difftime(tv_sec2, tv_sec) > total) break;
        else sleep(wait);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
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
    total = atoi(argv[4]);
    wait = atoi(argv[5]);
    if(strcmp(argv[6], "random") == 0) rnd = 1;
    else rnd = 0;
    int i;
    for(i=0; i<atoi(argv[3]); i++){
        pthread_create(&tid[i], NULL, process, NULL);
    }
    pthread_exit(NULL);

    // /* connect to server */

    // if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    //     error("ERROR connecting");

    // /* ask user for input */

    // printf("Please enter the message: ");
    // bzero(buffer,256);
    // fgets(buffer,255,stdin);

    // /* send user message to server */

    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // bzero(buffer,256);

    // /* read reply from server */
    // char buff[512];
    // while(1)
    // {
    //     n = read(sockfd,buff,512);
    //     if (n <= 0) 
    //     {
    //         //error("ERROR reading from socket");
    //         break;
    //     }     
    //     printf("%s\n",buff);
    // }
    // close(sockfd);
    //return 0;
}
