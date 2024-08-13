#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void prime(int input){
    int val=0;
    int prime_num=0;
    int p[2]={0};

    if(read(input,(char*)&prime_num,4)==4){
        printf("prime %d\n",prime_num);
        pipe(p);

        while(read(input,(char*)&val,4)==4){
            if(val%prime_num!=0){
                write(p[1],(char*)&val,4);
            }
        }
        close(input);
        close(p[1]);

        if(fork()==0){
            prime(p[0]);
        }
        else{
            wait(0);
            exit(0);
        }
    }
    else{
        exit(0);
    }
}

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    for(int i=2;i<=35;i++){
        write(p[1],&i,4);
    }
    close(p[1]);

    if(fork()==0){
        prime(p[0]);
    }
    wait(0);
    exit(0);
}