#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"
#include "kernel/fcntl.h"

void forkAndExec(int numTickets, int pids[], int idx) {
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork fail\n");
        exit(1);
    }
    
    if (pid == 0) { //child
        //execute ticket spam
        if (settickets(numTickets) == -1) {
            fprintf(2, "Could not set tickets to process.\n");
            exit(1);
        }
        while(1);
    } else { 
        //register child pid
        fprintf(2, "pid %d\n", pid);
        pids[idx] = pid;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4){
        fprintf(2, "testtickets usage: [number of csv lines] [number of assigned tickets to this program] [number of assigned tickets per instance..]\n");
        exit(1);
    }

    int fd;
    int pids[argc - 3];
    int idxs[argc - 3];
    int tickets[argc - 3];
    int runs = atoi(argv[1]);
    struct pstat pstat;

    if (settickets(atoi(argv[2])) == -1) {
            fprintf(2, "Could not set tickets to parent process.\n");
            exit(1);
        }

    if((fd = open("./testtickets.csv", O_CREATE|O_WRONLY)) < 0){
        printf("testtickets: cannot open testtickets.csv\n");
        exit(1);
    }

    for (int i = 0; i < argc - 3; i++) {
        tickets[i] = atoi(argv[i + 3]);

        if (i == argc - 4) fprintf(fd, "%dt_ticks\n", tickets[i], tickets[i]);
        else fprintf(fd, "%dt_ticks, ", tickets[i], tickets[i]);
    }

    for (int i=3; i < argc; i++) {
        //Run a ticketspam
        forkAndExec(tickets[i-3], pids, i - 3);

        //Find pstat index
        getpinfo(&pstat);
        int idx = 0;
        while (pstat.pid[idx] != pids[i-3]) idx++;
        idxs[i-3] = idx;
    }

    int tick_data[runs][argc - 3];
    for (int i=0; i < runs; i++) {
        printf("Reading %d\n", i);
        sleep(100);
        getpinfo(&pstat);

        //Record to matrix
        for (int j=0; j < argc - 3; j++) {
            tick_data[i][j] = pstat.ticks[idxs[j]];
        }
    }

    //Kill children.
    for (int i = 0; i < argc - 3; i++) {
        kill(pids[i]);
    }

    //Record data to csv
    for (int i=0; i < runs; i++) {
        for (int j=0; j < argc - 3; j++) {
            if (j == argc - 4) fprintf(fd, "%d\n", tick_data[i][j]);
            else fprintf(fd, "%d,", tick_data[i][j]);
        }
    }
    
    close(fd); //close file
    exit(0);
}
