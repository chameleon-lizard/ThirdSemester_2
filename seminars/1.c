#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>

int
main(void)
{
    int key = ftok("/", 's');
    int semid = semget(key, 1, IPC_CREAT | 0666);
    int shmid = shmget(key, 6 * sizeof(int), IPC_CREAT | 0666);

    semctl(semid, 0, SETVAL, 1);

    int *lol = shmat(shmid, NULL, 0);
    memset(lol, -1, sizeof(int) * 6);

    for (int i = 0; i < 5; i++) {
        if (!fork()) {
            struct sembuf d = {0, -1, SEM_UNDO};
            struct sembuf u = {0, 1, SEM_UNDO};

            if (i == 0) {
                // Crit
                semop(semid, &d, 1);

                lol[1] = i;

                semop(semid, &u, 1);

                printf("%d\n", lol[5]);
                _exit(0);
            }
            // Crit
            semop(semid, &d, 1);

            lol[i+1] = i;

            semop(semid, &u, 1);

            printf("%d\n", lol[i]);
            _exit(0);
        } else {
            continue;
        }
    }

    while (wait(NULL) != -1);
    semctl(semid, IPC_RMID, (int)0);

    return 0;
}