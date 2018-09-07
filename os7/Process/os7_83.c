#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//os concepts 7th
//P82

int main(int argc, char **argv)
{
	pid_t pid;
	pid = fork();
	if(pid < 0){	//failure
		perror("fork");
		exit(-1);
	}else if(pid == 0){	//child
		execlp("/bin/ls", "ls", NULL);
	}else{	//parent
		wait(NULL);	//wait until child is over 
		printf("Child complete.\n");
		exit(EXIT_SUCCESS);
	}
}
