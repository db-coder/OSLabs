1#include <unistd.h>
1#include <stdio.h>
1
1int main(int argc, char *argv[]) 
1
1{
1  unsigned int i,j;
1  while(1)
1    {
1      j = 1;
1      for(i = 1; i <= 10; i++)
1	{
1	  j = j*i;
1	}
1      printf("ten factorial is %u\n", j);
1    }
1}