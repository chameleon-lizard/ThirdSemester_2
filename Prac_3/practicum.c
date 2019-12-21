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
    for (int i = 1; i < N; i++) {
        kill(SIGTERM, children[i]);
    }
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
    int N = atoi(argv[1]);

    // Initializing array for pids, first int is the pid of father process.
    children = calloc(N, sizeof(*children));
    children[0] = getpid();

    // Creating semaphores to ensure there are no race conditions.
    key_t key = ftok(IPC_PRIVATE, 's');
    semid = semget(key, N * 2, IPC_CREAT | 0666);
    for (int i = 0; i < 2 * N; i++) {
        semctl(semid, i, SETVAL, 1);
    }

    // Creating shared memory.
    shmid = shmget(key, N * sizeof(int), IPC_CREAT | 0666);
    shm = shmat(shmid, NULL, 0);
    
    if (shm == (void *)-1) {
        perror("Memory error: ");
    }
    
    for (int i = 0; i < N; i++) {
        shm[i] = -1;
    }

    for(int i = 0; i < N; i++) {
        if (!(children[i] = fork())) {
            srand(i);
            if (N == 1) {
                printf("Only one caller.\n");
                _exit(0);
            }
            for (int j = 0; j < atoi(argv[2]); j++) {
                // Getting phone number.
                int number = 0;
                while ((number = rand() % (N)) == i);

                // For calls
                struct sembuf d[2] = {{number, -1, SEM_UNDO}, {i, -1, SEM_UNDO}};
                struct sembuf u[2] = {{number,  1, SEM_UNDO}, {i, 1, SEM_UNDO}};

                // For memory
                struct sembuf dm[2] = {{N + number, -1, SEM_UNDO}, {N + i, -1, SEM_UNDO}};
                struct sembuf um[2] = {{N + number,  1, SEM_UNDO}, {N + i, 1, SEM_UNDO}};

                // Critical part of the code.
                semop(semid, dm, 2);

                if (shm[number] == -1 && shm[i] == -1) {
                    semop(semid, d, 2);

                    shm[i] = number;
                    shm[number] = i;
                    printf("Call %d: %d\n", i, number);
                    printf("%d: %d\n", number, i);
                    fflush(stdout);
                    shm[number] = -1;
                    shm[i] = -1;
                    
                    semop(semid, u, 2);

                    semop(semid, um, 2);
                    printf("End call\n");
                } else {
                    semop(semid, um, 2);
                    j--;
                    continue;
                }
            }
            _exit(0);
        } else {
            continue;
        }
    }

    while (wait(NULL) != -1);
    free(children);
    shmdt(shm);
    semctl(semid, 0, IPC_RMID, (int)0);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}