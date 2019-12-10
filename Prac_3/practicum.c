#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <wait.h>
#include <time.h>

int *children;
int N;
int semid;
int shmid;
int *shm;


void
fsig(int sig) 
{
    for (int i = 0; i < N; i++) {
        kill(SIGTERM, children[i]);
    }

    // Freeing resources.
    free(children);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, (int)0);
    semctl(semid, IPC_RMID, (int)0);
}

int
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    // Creating N children, saving their pids to mercilessly kill them when the
    // time comes.
    N = atoi(argv[1]);
    N++;

    // Initializing array for pids.
    children = calloc(N, sizeof(*children));

    // Creating semaphores to ensure there are no race conditions.
    key_t key = ftok("/usr/bin/zsh", 's');
    semid = semget(key, N, IPC_CREAT | 0666);
    for (int i = 0; i < N; i++) {
        semctl(semid, 1, SETVAL, 1);
    }

    // Creating shared memory.
    shmid = shmget(key, N * sizeof(int), IPC_CREAT | 0666);
    int *shm = shmat(shmid, NULL, 0);
    memset(shm, 0, sizeof(int));

    for(int i = 0; i < N; i++) {
        if (!(children[i] = fork())) {
            srand(i);
            for (int j = 0; j < atoi(argv[2]); j++) {
                // Getting phone number.
                int number = 0;
                while ((number = rand() % (N)) == i);
                struct sembuf d[2] = {{number, -1, SEM_UNDO}, {i, -1, SEM_UNDO}};
                struct sembuf u[2] = {{number,  1, SEM_UNDO}, {i, 1, SEM_UNDO}};
                
                // Critical part of the code.
                semop(semid, d, 1);

                shm[i] = number;
                shm[number] = i;
                printf("Caller is: %d->%d\n", i, shm[i]);
                printf("Recipient is: %d<-%d\n", number, shm[number]);
                fflush(stdout);

                semop(semid, u, 1);
            }
            _exit(0);
        } else {
            continue;
        }
    }

    // Waiting for all processes to finish and freeing resources.
    while (wait(NULL) != -1);
    free(children);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, (int)0);
    semctl(semid, IPC_RMID, (int)0);

    return 0;
}