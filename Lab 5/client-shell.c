#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  	int i, tokenIndex = 0, tokenNo = 0;

  	for(i =0; i < strlen(line); i++)
  	{
	    char readChar = line[i];

	    if (readChar == ' ' || readChar == '\n' || readChar == '\t')
	    {
	      	token[tokenIndex] = '\0';
	    	if (tokenIndex != 0)
	    	{
				tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
		    }
	    } 
	    else {
	    	token[tokenIndex++] = readChar;
	    }
  	}
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


void  main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;
    char server_ip[200];
    char server_port[10];

    bzero(server_ip,200);
    strcpy(server_ip,"empty");

    bzero(server_port,10);
    strcpy(server_port,"empty");

    while (1) 
    {           
       
    	printf("Hello>");     
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
       // printf("Got command %s\n", line);
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
   		
       //do whatever you want with the commands, here we just print them

    	for(i=0;tokens[i]!=NULL;i++);
       
        if(strcmp(tokens[0],"cd")==0)
        {
        	if(i!=2)
        	{
        		fprintf(stderr,"Incorrect number of arguments!!!\n");
        		continue;
        	}
        	int x=chdir(tokens[1]);
        	if(x==-1)
        	{
        		fprintf(stderr, "Please give the correct path.\n");
        		continue;
        	}
        	char cwd[256];
        	getcwd(cwd,sizeof(cwd));
        	printf("%s\n",cwd);
        }
        else if(strcmp(tokens[0],"server")==0)
        {
        	if(i!=3)
        	{
        		fprintf(stderr,"Incorrect number of arguments!!!\n");
        		continue;
        	}
        	strcpy(server_ip,tokens[1]);
        	strcpy(server_port,tokens[2]);
        	printf("%s\n",server_port );
        }
        else if(strcmp(tokens[0],"getfl")==0)
        {
        	if(strcmp(server_ip,"empty")==0 || server_port==0)
        	{
        		printf("The values of server_ip and server_port must be set.\n");
        		continue;
        	}
            if(i==2)
            {
                pid_t pid;
                pid=fork();
                if(pid==0)
                {
                    char arg[100];
                    bzero(arg,100);
                    char arg1[100];
                    bzero(arg1,100);
                    strcpy(arg,"./get-one-file");
                    strcpy(arg1,"files/");
                    strcat(arg1,tokens[1]);
                    int err = execl(arg,arg,arg1,server_ip,server_port,"display",(char *)0);
                    if(err==-1)
                    {
                        fprintf(stderr, "Something went wrong :(\n");
                    }
                }
                else if(pid>0)
                {
                    int w;
                    do
                    {
                        w = waitpid(-1,0,WNOHANG);
                    }while(w!=-1);
                }
                else
                {
                    error("ERROR creating child process");
                }
            }
            else if(i==4 && strcmp(tokens[2],">")==0)
            {
                char *argv[2];
                argv[0]="/bin/cat";
                argv[1]=0;

                pid_t pid;
                pid=fork();
                if(pid==0)
                {
                    int fd = open(tokens[3],O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fd,1);
                    close(fd);

                    char arg[100];
                    bzero(arg,100);
                    char arg1[100];
                    bzero(arg1,100);

                    strcpy(arg,"./get-one-file");
                    strcpy(arg1,"files/");
                    strcat(arg1,tokens[1]);
                    int err = execl(arg,arg,arg1,server_ip,server_port,"display",(char *)0);
                    if(err==-1)
                    {
                        fprintf(stderr, "Something went wrong :(\n");
                    }
                }
                else if(pid>0)
                {
                    int w;
                    do
                    {
                        w = waitpid(-1,0,WNOHANG);
                    }while(w!=-1);
                }
            }
            else if(strcmp(tokens[2],"|")==0)
            {
                int p[2];
                
                char arg[100];
                bzero(arg,100);
                char arg1[100];
                bzero(arg1,100);

                strcpy(arg,"./get-one-file");
                strcpy(arg1,"files/");
                strcat(arg1,tokens[1]);

                pipe(p);

                pid_t pid;
                pid=fork();
                if(pid==0)
                {
                    close(p[0]);
                    dup2(p[1],1);
                    close(p[1]);
                    int err = execl(arg,arg,arg1,server_ip,server_port,"display",(char *)0);
                    if(err==-1)
                    {
                        fprintf(stderr, "Something went wrong :(\n");
                    }
                    // execl(arg,arg,(char *)0);
                }
                else
                {
                    char buff[1024];
                    close(p[1]);
                    while(read(p[0],buff,sizeof(buff))!=0)
                    {

                    }

                }
            }
            else
            {
                fprintf(stderr,"Incorrect number of arguments!!!\n");
                continue;
            }
        }
        else if(strcmp(tokens[0],"getsq")==0)
        {
        	
        }
        else if(strcmp(tokens[0],"getpl")==0)
        {
        	
        }
        else if(strcmp(tokens[0],"getbg")==0)
        {
        	
        }
        else if(strcmp(tokens[0],"exit")==0)
        {
        	exit(0);
        }
        else
        {
        	pid_t pid;
        	pid=fork();
        	if(pid==0)
        	{
        		char arg[100];
        		bzero(arg,100);
        		strcpy(arg,"/bin/");
        		strcat(arg,tokens[0]);
        		int err = execv(arg,tokens);
        		if(err==-1)
        		{
        			fprintf(stderr, "Something went wrong :(\n");
        		}
        	}
        	else if(pid>0)
	        {
	        	int w;
	      		do
	      		{
		            w = waitpid(-1,0,WNOHANG);
		        }while(w!=-1);
			}
	        else
	        {
	      		error("ERROR creating child process");
	        }
        }

       // Freeing the allocated memory	
    	for(i=0;tokens[i]!=NULL;i++)
    	{
			free(tokens[i]);
    	}
    	free(tokens);
     }
}

                
