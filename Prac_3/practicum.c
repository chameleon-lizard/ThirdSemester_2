#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include <time.h>

int
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    srand(time(NULL));

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
    for (int i = 0; i < N / 2; i++) {
        semctl(semid, 1, SETVAL, 1);
    }

    // Creating shared memory.
    int shmid = shmget(key, N * sizeof(int), IPC_CREAT | 0666);
    int *shm = shmat(shmid, NULL, 0);
    memset(shm, 0, sizeof(int));

    for(int i = 0; i < N; i++) {
        if (!(children[i] = fork())) {
            for (int j = 0; j < atoi(argv[2]); j++) {
                // Getting phone number.
                int number = -1;
                while ((number = rand() % (N) - 1) == i);
                struct sembuf d = {number / 2, -1, SEM_UNDO};
                struct sembuf u = {number / 2,  1, SEM_UNDO};
                
                // Critical part of the code.
                semop(semid, &d, 1);

                shm[i] = number;
                printf("%d: %d\n", i, number);
                fflush(stdout);
                sleep(1);
                shm[i] = 0;

                semop(semid, &u, 1);
            }
            _exit(0);
        } else {
            continue;
        }
    }

    while (wait(NULL) != -1);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, (int)0);
    semctl(semid, IPC_RMID, (int)0);

    return 0;
}