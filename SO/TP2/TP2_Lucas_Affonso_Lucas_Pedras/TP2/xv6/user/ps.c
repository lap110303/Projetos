#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"

// User space program that prints processes status
int main(int argc, char *argv[]) {
    struct pstat pstat;
    
    if (getpinfo(&pstat) == -1) {
        fprintf(2, "ps: could not get process info\n");
        exit(1);
    }

    printf("PID      TICKETS      TICKS\n");

    for (int i = 0; i < NPROC; i++) {
        if (pstat.inuse[i])
            printf("%d        %d            %d\n", pstat.pid[i], pstat.tickets[i], pstat.ticks[i]);
    }
        
    exit(0);
}