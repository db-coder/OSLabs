#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <signal.h>

long long int count=0;

void intHandler()
{
    fprintf(stderr,"Received SIGINT; downloaded %lld bytes so far.\n",count);
    exit(0);
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    signal(SIGINT,intHandler);

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    /* create socket, get sockfd handle */

    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address in sockaddr_in datastructure */

    server = gethostbyname(argv[2]);
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

    /* connect to server */

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    /* ask user for input */
    // printf("%s %s %s %s\n",argv[1],argv[2],argv[3],argv[4] );
    bzero(buffer,256);
    strcpy(buffer,argv[1]);

    int disp;
    if (strcmp(argv[4],"display")==0)
    {
    	disp=1;
    }
    else
    {
    	disp=0;
    }
    /* send user message to server */

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);

    /* read reply from server */
    char buff[1024];
    while(1)
    {
        n = read(sockfd,buff,1024);
        if (n <= 0) 
        {
            break;
        }
        count+=n;
        if (disp)
        {
         	printf("%s",buff);
        }
    }
    close(sockfd);
    return 0;
}