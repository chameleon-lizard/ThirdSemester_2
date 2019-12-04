#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    // Creating N children, saving their pids to mercilessly kill them when the
    // time comes.
    int N = atoi(argv[1]);
    N++;

    // Initializing array for pids, first int is the pid of father process.
    int *children = calloc(N, sizeof(*children));
    children[0] = getpid();

    for(int i = 0; i < N; i++) {
        if (!(children[i] = fork())) {
            break;
        } else {
            continue;
        }
    }

    

    free(children);

    return 0;
}