#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// User space program that uses the getcnt syscall
int main(int argc, char *argv[]) {
    if (argc != 2){
        fprintf(2, "Usage: getcnt [syscall number]\n");
        exit(1);
    }

    int cnt = getcnt(atoi(argv[1]));

    if (cnt == -1)
        fprintf(2, "getcnt: syscall %s does not exist\n", argv[1]);
    else
        printf("syscall %d has been called %d times\n", atoi(argv[1]), cnt);
        
    exit(0);
}