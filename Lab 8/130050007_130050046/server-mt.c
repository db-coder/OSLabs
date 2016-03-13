/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

struct file_desc
{
    int sock_fd_no;
    struct file_desc * next;
};

struct file_desc *top;
struct file_desc *current;
struct file_desc *fd_new;
struct file_desc *temp;

int requests=0,flag=1;
pthread_mutex_t mutex;
pthread_cond_t fill,full;

void *process()
{
    while(1)
    {
        int sock;
        int n;
        char buffer[256];   
        bzero(buffer,256);
        pthread_mutex_lock(&mutex);
        while(requests==0)
            pthread_cond_wait(&fill,&mutex);
        sock=top->sock_fd_no;
        temp = top->next;
        free(top);
        requests--;
        top = temp;
        if(top==NULL)
            flag=1;
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        n = read(sock,buffer,255);
        if (n < 0) 
            error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        char buffer1[256];
        bzero(buffer1,256);
        strcpy(buffer1,buffer+4);

        size_t len = strlen(buffer1);
        char * newBuf = (char *)malloc(len);
        memcpy(newBuf,buffer1,len);
        FILE *fp;
        fp = fopen(newBuf,"r");
        char buff[1024];
        bzero(buff,1024);
        while(fgets(buff,1024,(FILE*)fp) != NULL)
        {
            n = write(sock,buff,1024);
            bzero(buff,1024);
            /* send reply to client */
            if (n < 0) 
                error("ERROR writing to socket");
        }
        fclose(fp);
        close(sock);
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    current = calloc(1,sizeof(struct file_desc));
    current->next = NULL;
    top = current;
    int portno, clilen, pid, w, no_threads, newsockfd,sockfd;
    if (argc < 4) 
    {
     fprintf(stderr,"ERROR, incorrect no. of arguments provided\n");
     exit(1);
    }

    /* create socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY) */

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    /* listen for incoming connection requests */

    listen(sockfd,500);
    clilen = sizeof(cli_addr);

    /* creating threads */
    no_threads = atoi(argv[2]);
    int limit= atoi(argv[3]);
    pthread_t tid[no_threads];
    int i;
    for(i=0; i<no_threads; i++)
    {
        pthread_create(&tid[i], NULL, process, NULL);
    }

    while(1)
    {
        /* accept a new request, create a newsockfd */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        pthread_mutex_lock(&mutex);
        while(limit < requests && limit!=0)
            pthread_cond_wait(&full,&mutex);
        fd_new = calloc(1,sizeof(struct file_desc));
        fd_new->sock_fd_no = newsockfd;
        fd_new->next = NULL;
        if(flag)
        {
            current = fd_new;
            top=current;
            flag=0;
        }  
        current->next = fd_new;
        current = current->next;
        requests++;
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }
    return 0; 
}