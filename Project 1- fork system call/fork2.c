/* 	Fatmagül İLKER
 * */


#include <stdio.h> //printf, scanf
#include <stdlib.h> //exit
#include <unistd.h> //fork, getpid, getppid
#include <sys/wait.h> //wait

void right_subtree(int,int);
void left_subtree(int);

int main(int argc, char *argv[]){
	int n_right;
	int m_left;
	printf("Enter the N number: ");
	scanf("%d",&n_right);
	printf("Enter the M number: ");
	scanf("%d",&m_left);
	printf("Root: %d \n",getpid());
	right_subtree(n_right,m_left);
	return 0;

}
void right_subtree(int depth, int m_number)
 { //depth is N number here
	if(depth==0)
	{ //for the last right child
		left_subtree(m_number); 
		return;
	}
	else
	{
		int rc=fork(); //right child is created
		if(rc<0)
		{ //error
			fprintf(stderr, "fork failed\n");
			exit(1);
		} 
		if(rc==0)
		{ //right child process
			printf("Right child. PID: %d, Parent's PID: %d \n",getpid(), getppid());
			right_subtree(--depth,m_number); //to create next right child.
		}
		else{ //parent process 
			wait(NULL);
			left_subtree(m_number); //left child are created with tih M number
		}
	}
}

void left_subtree(int depth)
{
	if(depth==0){ return; }
	else{
		int lc=fork(); //first left child İS created
		if(lc<0)
		{
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		else if(lc==0)
		{ //left child process
			printf("Left child. PID: %d, Parent's PID: %d \n",getpid(), getppid());
			left_subtree(--depth);
		}
		else
		{ //parent waits for the child
				wait(NULL);
		}
	}
}
