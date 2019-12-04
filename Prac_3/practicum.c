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

    // Creating semaphores to ensure there are no race conditions.
    key_t key = ftok("/usr/bin/zsh", 's');
    int semid = semget(key, N / 2, IPC_CREAT | 0666);

    // Creating shared memory.
    int shmid = shmget(key, N / 2 * sizeof(char), IPC_CREAT | 0666);
    int *shm = shmat(shmid, NULL, 0);

    for(int i = 0; i < N; i++) {
        if (!(children[i] = fork())) {
            // Children gotta talk.
            struct sembuf d = {i, -1, SEM_UNDO};
            struct sembuf u = {i,  1, SEM_UNDO};
            semop(semid, &d, 1);
            // Critical part of the code.
            
            semop(semid, &u, 1);
        } else {
            continue;
        }
    }

    free(children);

    return 0;
}