#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COMMAND_SIZE 1000
#define MAX_ARGS 50
#define NAMES_SIZE 500
#define TRUE 1

void print_prompt();
void read_command(char *, char *[]);
void execute_command(char *, char *[]);

int main(int argc, char const *argv[])
{
  char command[COMMAND_SIZE]; 
	char *arglist[MAX_ARGS];

	do{
		print_prompt();
		read_command(command, arglist);
		execute_command(command, arglist);
	}while(TRUE);
}

void print_prompt()
{
	char hostname[NAMES_SIZE];
	char current_dir[NAMES_SIZE];	
	char *home_dir = getenv("HOME");

	gethostname(hostname,NAMES_SIZE);
	getcwd(current_dir, NAMES_SIZE);

	//changing /home/user to ~
	if (strncmp(home_dir, current_dir, strlen(home_dir))==0){
		int i=1;
		int j = strlen(home_dir);
		while(j<strlen(current_dir)){
			current_dir[i++] = current_dir[j++];
		}
		current_dir[i]='\0';
		current_dir[0]='~';
	}

	//printing the prompt
	printf("[MySh] %s@%s:%s$ ",
		getenv("LOGNAME"),
		hostname,
		current_dir);
}

void read_command(char *command, char *arglist[])
{	
	//fgets is safer than gets
	fgets(command, COMMAND_SIZE, stdin); 

	//removing right spaces
	int i = strlen(command)-1;
	while(isspace(command[i])){
		command[i--]='\0';
	}

	//getting arguments
	arglist[i=0] = strtok(command," ");
	while(arglist[i]!=NULL && i<MAX_ARGS){
		arglist[++i] = strtok(NULL," ");	
	}
	
}

void execute_command(char *command, char *arglist[])
{	
	if (strcmp(command,"exit")==0)
		exit(0);

	//executing commands
	pid_t pid = fork();
	if (pid < 0){
		printf("Error");
	}else if (pid > 0)	{ //father
		int status;
		wait(&status);	//waiting for child execution
	}else{	//execunting child
		execvp(arglist[0], arglist);
		fprintf (stderr, "Não foi possível executar \"%s\"\n",command);
		abort ();
	}	
}
