#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
void run_mapper(int fd[]); 
void run_reducer(int fd[]); 

int main() {
	pid_t pid; 
	int fd[2], status;
	pipe(fd); 
	run_reducer(fd); 
	run_mapper(fd); 	
	close(fd[0]); //Close both the read and write ends of the pipe of
	close(fd[1]); //the parent
	wait(&status);
	wait(&status);// parent waits for the children to complete
	exit(0);
}

void run_mapper(int fd[]) //runs the mapper process
{ 	
	char *map[]={"./mapper",0}; 
	pid_t pid; 
	pid = fork();
	if(pid==0){ 
		dup2(fd[1],1); 
		close(fd[0]); 
		execvp(map[0], map); 
		printf("Exec error\n");
	}
	else if(pid<0){
		printf("Error in creating mapper child"); 
		exit(0); 
	}
	
}	

void run_reducer(int fd[]) //runs the reducer process
{ 	
	char *cmd2[]={"./reducer",0}; 
	pid_t pid; 
	pid = fork();
	if(pid==0){ 
		dup2(fd[0],0); 
		close(fd[1]); 
		execvp(cmd2[0], cmd2); 
		printf("Exec error\n");
	}	
	else if(pid<0){
		printf("Error in creating reducer child"); 
		exit(0); 
	}
} 
