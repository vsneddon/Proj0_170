#include <stdio.h>
#include <unistd.h>

int main(){
    if(fork() == 0){// child
        printf("child \n");
    }
    else if(fork() == 0){ //second child
        printf("grandchild \n");
    }
    else{ //parent
        printf("parent \n");
    }

    return 0;
}
