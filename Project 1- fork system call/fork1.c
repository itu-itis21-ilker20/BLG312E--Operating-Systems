/* 	Fatmagül İLKER
 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void process_tree(int);

int main(int argc, char *argv[]){
	int depth;
	printf("Enter the N number: ");
	scanf("%d",&depth);
	printf("Root: %d \n",getpid());
	process_tree(depth);
	return 0;

}

void process_tree(int depth)
{
	int lc=fork(); //left child is created
	if(lc<0)
	{
		fprintf(stderr, "fork failed\n");
		exit(1);
	} 
	if(lc==0)
	{ //left child process
		printf("Left child. PID: %d, PID: %d \n",getpid(), getppid());
	}
	else{ //parent process 
		wait(NULL); //first waits for the left child to finish its process
		if(depth==0){ return; } 
		else
		{
			int rc=fork(); //right child created
			if(rc<0)
			{
				fprintf(stderr, "fork failed\n");
				exit(1);
			}
			else if(rc==0)
			{ //right child process
				printf("Right child. PID: %d, Parent's PID: %d \n",getpid(), getppid());
				process_tree(--depth);
			}
			else
			{ //parent process waits for the child
				wait(NULL);
			}
		}
	}	
}
