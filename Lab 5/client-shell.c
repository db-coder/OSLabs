#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    int server_port=0;

    bzero(server_ip,200);
    strcpy(server_ip,"empty");

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
        	server_port=atoi(tokens[2]);
        	printf("%d\n",server_port );
        }
        else if(strcmp(tokens[0],"getfl")==0)
        {
        	if(i!=2)
        	{
        		fprintf(stderr,"Incorrect number of arguments!!!\n");
        		continue;
        	}
        	if(strcmp(server_ip,"empty")==0 || server_port==0)
        	{
        		printf("The values of server_ip and server_port must be set.\n");
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
        		int err = execl(arg,arg,tokens[1],(char *)0);
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

                
