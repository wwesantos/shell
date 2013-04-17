#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
 
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"
#define COMMAND_SIZE 1000
#define MAX_ARGS 50
#define NAMES_SIZE 500
#define TRUE 1
#define FALSE 0
#define MATCH 0
 
sig_atomic_t sig_flag = 0;
 
void print_prompt();
void read_command(char *[]);
void execute_command(char *[]);
void execute_command_cd(char *[]);
void signal_init(struct sigaction);
void signal_handler(int);
 
int main(int argc, char const *argv[])
{
    struct sigaction sa;
    signal_init(sa);
    char *command_arglist[MAX_ARGS];
 
    do{
        print_prompt();
        read_command(command_arglist);
        execute_command(command_arglist);
    }while(TRUE);
}
 
void print_prompt()
{
    char *home_dir = getenv("HOME");
    char current_dir[NAMES_SIZE];   
    getcwd(current_dir, NAMES_SIZE);
 
    //changing /home/user for ~
    if (strncmp(home_dir, current_dir, strlen(home_dir))==MATCH){
        strcpy(current_dir,
            current_dir+strlen(home_dir)-sizeof(char));
        current_dir[0]='~';
    }
 
    //printing prompt
    char hostname[NAMES_SIZE];
    gethostname(hostname, NAMES_SIZE);
    printf(KGRN"[MySh] %s@%s:%s$ "KNRM, 
        getenv("LOGNAME"), hostname, current_dir);
}
 
void read_command(char *command_arglist[])
{   
    char command[COMMAND_SIZE];
    fgets(command, COMMAND_SIZE, stdin); //fgets is safer than gets
 
    //removing right spaces
    int i = strlen(command)-1;
    while(isspace(command[i])) 
        command[i--]='\0';
 
    //getting arguments
    command_arglist[i=0] = strtok(command," ");
    while(command_arglist[i]!=NULL && i<MAX_ARGS)
        command_arglist[++i] = strtok(NULL," ");
     
}
 
void execute_command(char *command_arglist[])
{   
    if(sig_flag){ //checking if received signal
        printf("\n");
        sig_flag = 0;
        return;
    }else if (strcmp(command_arglist[0],"exit")==MATCH){
        exit(0);
    }else if(strcmp(command_arglist[0],"cd")==MATCH){
        execute_command_cd(command_arglist);
    }else{
        pid_t pid = fork();
        if (pid < 0){
            printf("Error");
        }else if (pid > 0)   { //father
            int status;
            wait(&status);  //waiting for child execution
        }else{  //execunting child
            execvp(command_arglist[0], command_arglist);
            fprintf (stderr, 
                "NÃ£o foi possÃ­vel executar \"%s\", erro: %s\n",
                command_arglist[0],
                strerror(errno));
            abort ();
        }
    }
}
 
void execute_command_cd(char *command_arglist[])
{
    if (command_arglist[1]==NULL){
        chdir(getenv("HOME"));
    }else if(chdir(command_arglist[1])<0){
        fprintf (stderr,
            "Erro ao executar \"%s\": %s\n",
            command_arglist[0],
            strerror(errno));
    }
         
}
 
void signal_init(struct sigaction sa)
{
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &signal_handler;
    sigaction (SIGTSTP, &sa, NULL); //Ctrl+Z
    sigaction (SIGINT, &sa, NULL); //Ctrl+C
 
}
 
void signal_handler (int signal_number)
{
    sig_flag = 1;
}
