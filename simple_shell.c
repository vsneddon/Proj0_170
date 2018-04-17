#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 50
#define MAX_TOKEN_COUNT 100
#define MAX_LINE_LENGTH 512

// Simple implementation for Shell command
// Assume all arguments are seperated by space
// Erros are ignored when executing fgets(), fork(), and waitpid().

/**
 * Sample session
 *  shell: echo hello
 *   hello
 *   shell: ls
 *   Makefile  simple  simple_shell.c
 *   shell: exit
 **/

void quitter(){
    exit(0);
}
void handler(int sig){
    signal(SIGTSTP, quitter);
}

void executeCommand();
void redirectHandler();


//Separate commands by pipe and run each command
void pipeHandler(char** args){
    int numcmds = 1;
    int z = 0;
    while(args[z] != NULL){
        if(strcmp(args[z], "|") == 0){
            numcmds++;
        }
        z++;
    }

    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);

    if(fork() == 0){ //Child or cmd 2
        if(numcmds > 1){
            //Read from command 1
            dup2(fd1[0], 0);
            close(fd1[1]);
            if(numcmds == 2){ //close pipes for command 3
                close(fd2[0]);
                close(fd2[1]);
            }
            else if(numcmds == 3){
                dup2(fd2[1], 1);
                close(fd2[0]);
            }
            executeCommand();
        }

    }else if (fork() == 0){ //Grandchild or cmd 3
        if(numcmds == 3){
            //Close access to pipe 1
            close(fd1[0]);
            close(fd1[1]);
            //Read from pipe 2
            dup2(fd2[0], 0);
            close(fd2[1]);
            executeCommand();
        }

    }else{ // parent or cmd1
        if(numcmds > 0){
            if(numcmds > 1){ //pipe to second cmd
                dup2(fd1[1], 1);
                close(fd1[0]);
            }else{ //close for no piping
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);
            }
            executeCommand(args);
        }
    }

}


//Split process into parent, and child where child executes commands
void runcommands(char** args) {
    pid_t pid = fork();
    if(pid) { // parent
        waitpid(pid, NULL, 0);
    } else { // child
        //execvp(command, args);
        pipeHandler(args);
    }
}

int main(){
    char line[MAX_LINE_LENGTH];
    //printf("shell: ");

    signal(SIGTSTP, handler);
    while(fgets(line, MAX_LINE_LENGTH, stdin)) {
        // Build the command and arguments, using execv conventions.
        line[strlen(line)-1] = '\0'; // get rid of the new line
        char* arguments[MAX_TOKEN_COUNT];
        int argument_count = 0;

        char* token = strtok(line, " ");
        while(token) {
            arguments[argument_count] = token;
            argument_count++;
            token = strtok(NULL, " ");
        }
        arguments[argument_count] = NULL;

        if(argument_count>0) {
            if (strcmp(arguments[0], "exit") == 0) exit(0);
            runcommands(arguments);
        }
        //printf("shell: ");
    }
    return 0;
}
