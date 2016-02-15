#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int num=0;
int bg_id[64];
int pg_id=0;
int first=1;
int die=0;
int getsq=0;

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

void childhandler(){
    pid_t pid;
    int i;
    for (i = 0; i < 64; ++i)
    {
        if(bg_id[i]!=-1)
        {
            pid = waitpid(bg_id[i],0,WNOHANG);
            if(pid==bg_id[i])
            {
                printf("Background process exited\n");
            }
        }
    }
}

void inthandler()
{
    killpg(pg_id,SIGINT);
    first=1;
    if(getsq==1)
        die=1;
}

int main()
{
    int parent_pid=getpid();
    int i;
    for (i = 0; i < 64; ++i)
    {
        bg_id[i]=-1;
    }
    signal(SIGINT,inthandler);
    signal(SIGCHLD, childhandler);
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
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
        
        if(i==0)
            continue;
        else if(strcmp(tokens[0],"cd")==0)
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
                if(first==1)
                {
                    pg_id=pid;
                    first=0;
                }
                setpgid(pid,pg_id);
                if(pid==0)
                {
                    char arg[100];
                    bzero(arg,100);
                    char arg1[100];
                    bzero(arg1,100);
                    strcpy(arg,"./get-one-file-sig");
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
                    waitpid(pid,0,0);
                }
                else
                {
                    fprintf(stderr, "ERROR creating child process(\n");
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

                    strcpy(arg,"./get-one-file-sig");
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
                    if(first==1)
                    {
                        pg_id=pid;
                        first=0;
                    }
                    setpgid(pid,pg_id);
                    waitpid(pid,0,0);
                }
            }
            else if(strcmp(tokens[2],"|")==0)
            {
                int p[2];
                
                char arg[100];
                bzero(arg,100);
                char arg1[100];
                bzero(arg1,100);

                strcpy(arg,"./get-one-file-sig");
                strcpy(arg1,"files/");
                strcat(arg1,tokens[1]);

                pipe(p);

                pid_t pid;
                pid=fork();
                if(pid<0)
                {
                    fprintf(stderr, "Child couldn't be created :(\n");
                }
                if(pid==0)
                {
                    close(1);
                    dup(p[1]);
                    close(p[0]);
                    close(p[1]);
                    int err = execl(arg,arg,arg1,server_ip,server_port,"display",(char *)0);
                    if(err==-1)
                    {
                        fprintf(stderr, "Something went wrong :(\n");
                    }
                }
                else
                {
                    if(first==1)
                    {
                        pg_id=pid;
                        first=0;
                    }
                    setpgid(pid,pg_id);
                    pid_t pid1;
                    pid1=fork();
                    if(pid1<0)
                    {
                        fprintf(stderr, "Child couldn't be created :(\n");
                    }
                    if(pid1==0)
                    {
                        close(0);
                        dup(p[0]);
                        close(p[0]);
                        close(p[1]);
                        bzero(arg,100);
                        strcpy(arg,"/bin/");
                        strcat(arg,tokens[3]);
                        int err = execl(arg,arg,tokens[4],(char *)0);
                        if(err==-1)
                        {
                            fprintf(stderr, "Something went wrong :(\n");
                        }
                    }
                    setpgid(pid1,pg_id);
                    close(p[0]);
                    close(p[1]);
                    int w,w2;
                    do
                    {
                        w = waitpid(pid1,0,0);
                    }while(w!=-1) ;
                    do
                    {
                        w2 = waitpid(pid,0,0);
                    }while(w2!=-1) ;
                }
                close(p[0]);
                close(p[1]);
            }
            else
            {
                fprintf(stderr,"Incorrect number of arguments!!!\n");
                continue;
            }
        }
        else if(strcmp(tokens[0],"getsq")==0)
        {
            getsq=1;
            if(strcmp(server_ip,"empty")==0 || server_port==0)
            {
                printf("The values of server_ip and server_port must be set.\n");
                continue;
            }
        	if(i<2){
                fprintf(stderr,"Incorrect number of arguments!!!\n");
                continue;
            }
            int j;
            for(j=1; j<i; j++)
            {
                if(die!=1)
                {
                    pid_t pid;
                    pid=fork();
                    if(pid==0)
                    {
                        char arg[100];
                        bzero(arg,100);
                        char arg1[100];
                        bzero(arg1,100);
                        strcpy(arg,"./get-one-file-sig");
                        strcpy(arg1,"files/");
                        strcat(arg1,tokens[j]);
                        int err = execl(arg,arg,arg1,server_ip,server_port,"nodisplay",(char *)0);
                        if(err==-1)
                        {
                            fprintf(stderr, "Something went wrong :(\n");
                        }
                    }
                    else if(pid>0)
                    {
                        if(first==1)
                        {
                            pg_id=pid;
                            first=0;
                        }
                        setpgid(pid,pg_id);
                        waitpid(pid,0,0);
                    }
                    else
                    {
                        fprintf(stderr, "ERROR creating child process(\n");
                    }
                }
            }  
            die=0;
            getsq=0; 
        }
        else if(strcmp(tokens[0],"getpl")==0)
        {
            int *pl_id = (int *)malloc((i-1) * sizeof(int));
            if(strcmp(server_ip,"empty")==0 || server_port==0)
            {
                printf("The values of server_ip and server_port must be set.\n");
                continue;
            }
        	if(i<2){
                fprintf(stderr,"Incorrect number of arguments!!!\n");
                continue;
            }
            int j;
            for(j=0; j<i-1; j++)
            {
                pl_id[j]=fork();
                if(pl_id[j]==0)
                {
                    char arg[100];
                    bzero(arg,100);
                    char arg1[100];
                    bzero(arg1,100);
                    strcpy(arg,"./get-one-file-sig");
                    strcpy(arg1,"files/");
                    strcat(arg1,tokens[j+1]);
                    int err = execl(arg,arg,arg1,server_ip,server_port,"nodisplay",(char *)0);
                    if(err==-1)
                    {
                        fprintf(stderr, "Something went wrong :(\n");
                    }
                    exit(0);
                }
                else if(pl_id[j]<0)
                {
                    fprintf(stderr, "ERROR creating child process(\n");
                }
                if(first==1)
                {
                    pg_id=pl_id[j];
                    first=0;
                }
                setpgid(pl_id[j],pg_id);
            }
            int l;
            for (l = 0; l < i-1;l++)
            {
                waitpid(pl_id[l],NULL,0);
            }  
            free(pl_id);
        }
        else if(strcmp(tokens[0],"getbg")==0)
        {
        	if(strcmp(server_ip,"empty")==0 || server_port==0)
            {
                printf("The values of server_ip and server_port must be set.\n");
                continue;
            }
            if(i!=2)
            {
                fprintf(stderr,"Incorrect number of arguments!!!\n");
                continue;
            }
            pid_t pid;
            pid=fork();
            if(pid==0)
            {
                char arg[100];
                bzero(arg,100);
                char arg1[100];
                bzero(arg1,100);
                strcpy(arg,"./get-one-file-sig");
                strcpy(arg1,"files/");
                strcat(arg1,tokens[1]);
                int err = execl(arg,arg,arg1,server_ip,server_port,"nodisplay",(char *)0);
                if(err==-1)
                {
                    fprintf(stderr, "Something went wrong :(\n");
                }
            }
            else if(pid>0)
            {
                bg_id[num]=pid;
                setpgid(pid,bg_id[0]);
                num++;
            }
            else
            {
                fprintf(stderr, "ERROR creating child process(\n");
            }
        }
        else if(strcmp(tokens[0],"exit")==0)
        {
            if(bg_id[0]!=-1)
                killpg(bg_id[0],SIGINT);
            free(tokens);
            exit(0);
        }
        else
        {
            int err=0;
        	pid_t pid;
        	pid=fork();
        	if(pid==0)
        	{
        		char arg[100];
        		bzero(arg,100);
        		strcpy(arg,"/bin/");
        		strcat(arg,tokens[0]);
        		if(execvpe(arg,tokens))
        		{
        			printf("Something went wrong :(\n");
                    exit(0);
        		}
        	}
        	else if(pid>0)
	        {
                if(first==1 && err!=-1)
                {
                    pg_id=pid;
                    first=0;
                }
                setpgid(pid,pg_id);
                waitpid(pid,0,0);  
			}
	        else
	        {
	      		fprintf(stderr, "ERROR creating child process(\n");
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