#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    if(fork() == 0){// child
        if(fork()==0){
            printf("grandchild \n");
        }
        else{
            waitpid(-1,NULL, 0);
            printf("child \n");
        }
    }
    else{ //parent
        waitpid(-1, NULL, 0);
        printf("parent \n");
    }

    return 0;
}
