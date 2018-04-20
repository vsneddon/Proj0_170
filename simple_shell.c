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

void signalQuitter(){
    exit(0);
}
void signalHandler(int sig){
    signal(SIGTSTP, signalQuitter);
}

void runcommand(char** args){
    execvp(args[0], args);
    perror(args[0]);
    exit(1);
}


//Separate commands by pipe and run each command
void pipeHandler(char** args){
    char* cmd1[MAX_TOKEN_COUNT];
    char* cmd2[MAX_TOKEN_COUNT];
    char* cmd3[MAX_TOKEN_COUNT];


    int cmdpos1 = 0;
    int cmdpos2 = 0;
    int cmdpos3 = 0;
    int numcmds = 1;
    int z = 0;

    //Add arguments to cmd lists
    while(args[z] != NULL){
        if(strcmp(args[z], "|") == 0){
            numcmds++;
        }else{
            switch(numcmds){
                case 1:{
                    cmd1[cmdpos1] = args[z];
                    cmdpos1++;
                    break;
                }case 2:{
                    cmd2[cmdpos2] = args[z];
                    cmdpos2++;
                    break;
                }case 3:{
                    cmd3[cmdpos3] = args[z];
                    cmdpos3++;
                    break;
                }
            }
        }
        z++;
    }
    cmd1[cmdpos1] = NULL;
    cmd2[cmdpos2] = NULL;
    cmd3[cmdpos3] = NULL;

    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);

    if(fork() == 0){//child1 cmd1
        if(numcmds > 0){
            if(numcmds > 1){ //pipe to second cmd
                dup2(fd1[1], 1);
            }

            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            runcommand(cmd1);
        } else{exit(0);}
    }
    else if(fork() == 0){//child2 cmd 2
        if(numcmds > 1){
            //Read from command 1
            dup2(fd1[0], 0);
            if(numcmds == 3){
                dup2(fd2[1], 1);
            }

            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            runcommand(cmd2);
        }else{exit(0);}

    }
    else{ //parent or cmd 3
        if(numcmds == 3){
            //Read from pipe 2
            dup2(fd2[0], 0);

            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            runcommand(cmd3);
        }else{exit(0);}
    }

    // if(fork() == 0){
    //     //command 1
    //     dup2(fd1[1], 1);
    //
    //     close(fd1[0]);
    //     close(fd1[1]);
    //     close(fd2[0]);
    //     close(fd2[1]);
    //
    //     runcommand(cmd1);
    //
    // }else{
    //     if(fork() == 0){
    //         //command2
    //         dup2(fd1[0], 0);
    //         dup2(fd2[1], 1);
    //
    //         close(fd1[0]);
    //         close(fd1[1]);
    //         close(fd2[0]);
    //         close(fd2[1]);
    //         runcommand(cmd2);
    //
    //     }else{
    //         if(fork() == 0){
    //             //command 3
    //             dup2(fd2[0], 0);
    //
    //             close(fd1[0]);
    //             close(fd1[1]);
    //             close(fd2[0]);
    //             close(fd2[1]);
    //             runcommand(cmd3);
    //         }
    //     }
    // }
    // close(fd1[0]);
    // close(fd1[1]);
    // close(fd2[0]);
    // close(fd2[1]);
    // waitpid(-1, NULL, 0);
    // exit(0);
}


void runCommands(char** args){
    pid_t pid = fork();
    if(pid) { // parent
        waitpid(pid, NULL, 0);
    } else { // child
        pipeHandler(args);
    }
}

int main(){
    char line[MAX_LINE_LENGTH];
    //printf("shell: ");

    signal(SIGTSTP, signalHandler);
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
            runCommands(arguments);
        }
        //printf("shell: ");
    }
    return 0;
}
