#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    char byte[1] = {'a'};
    char buf[1];
    int status[1];
    int p[2];
    pipe(p);

    if(fork()==0){
        close(p[1]);
        if(read(p[0],buf,1)==1 && buf[0]==byte[0]){
            printf("%d: received ping\n",getpid());
            exit(0);
        }
        exit(1);
    }
    else{
        close(p[0]);
        write(p[1],byte,1);
        wait(status);
        if(status[0]==0){
            printf("%d: received pong\n",getpid());
        }
        else{
            printf("child process erro\n");
        }
    }
    exit(0);
}