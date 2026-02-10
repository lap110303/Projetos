#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"

int main(int argc, char *argv[]) {
    if (argc != 2){
        fprintf(2, "Usage: ticketspam [number of assigned tickets]\n");
        exit(1);
    }

    int res = settickets(atoi(argv[1]));

    if (res == -1) {
        fprintf(2, "ticketspam: could not set tickets to process. Number of tickets cannot be smaller than 1\n");
        exit(1);
    }
    else {
        for (;;);
    }
    
    exit(0);
}