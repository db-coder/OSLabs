/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen,pid,w,status;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
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

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     /* listen for incoming connection requests */

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     /* accept a new request, create a newsockfd */

     while(1)
     {
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
              error("ERROR on accept");

          printf("connection\n");

         while ((w = waitpid(-1,&status,WNOHANG)) > 0)                               
         	fprintf(stderr,"Completed: %d (%d)\n",w,status);

         /* read message from client */
      	pid = fork();
      	if(pid==0)
      	{
      		close(sockfd);
	         bzero(buffer,256);
	         n = read(newsockfd,buffer,255);
	         if (n < 0) error("ERROR reading from socket");
	         printf("Here is the message: %s\n",buffer);
	         char buffer1[256];
	         bzero(buffer1,256);
	         strcpy(buffer1,buffer+4);

	         size_t len = strlen(buffer1);
	         char * newBuf = (char *)malloc(len);
	         memcpy(newBuf,buffer1,len);
             printf("file:%s\n",newBuf );
	         FILE *fp;
	         fp = fopen(newBuf,"r");
	         char buff[512];
	         while(fgets(buff,512,(FILE*)fp) != NULL)
	         {
	            n = write(newsockfd,buff,512);
	            printf("%s\n",buff);
	            /* send reply to client */
	            if (n < 0) 
	                error("ERROR writing to socket");
	         }
	         fclose(fp);
	         close(newsockfd);
	         exit(0);
	      }
	      else if(pid>0)
	      {
	      	close(newsockfd);
	      	continue;
	      }
	      else
	      {
	      	error("ERROR creating child process");
	      }
     }
     return 0; 
}
