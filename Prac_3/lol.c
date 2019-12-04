#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    int count = atoi(argv[1]);
    int key = atoi(argv[2]);
    int nproc = atoi(argv[3]);
    int iter_count = atoi(argv[4]);

    int semid = semget(key, count, IPC_CREAT | 0666);

    for (int i = 0; i < count; i++) {
        semctl(semid, i, SETVAL, 1);
    }

    int shmid = shmget(key, count * sizeof(int), IPC_CREAT | 0666);
    int *shm = shmat(shmid, NULL, 0);

    for (int i = 0; i < count; i++) {
        scanf("%d", &shm[i]);
    }
    
    for (int i = 0; i < nproc; i++) {
        if (!fork()) {
            unsigned seed = atoi(argv[5 + i]);
            srand(seed);
            for (int j = 0; j < iter_count; j++) {
                int ind1 = rand() / (RAND_MAX + 1.0) * count;
                int ind2 = rand() / (RAND_MAX + 1.0) * count;
                int value = rand() / (RAND_MAX + 1.0) * 10;
                 
                if (ind1 != ind2) {
                    struct sembuf p[2] = {{ind1, -1, SEM_UNDO}, {ind2, -1, SEM_UNDO}};
                    struct sembuf v[2] = {{ind1, 1, SEM_UNDO}, {ind2, 1, SEM_UNDO}};
                    semop(semid, p, 2);
                    operation(shm, ind1, ind2, value);
                    semop(semid, v, 2);
                }
            }
            return 0;
        }
    }
    while(wait(NULL) != -1);
    for (int i = 0; i < count; i++) {
        printf("%d ", shm[i]);
        fflush(stdout);
    }
    printf("\n");
    semctl(semid, 0, IPC_RMID, 0);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}