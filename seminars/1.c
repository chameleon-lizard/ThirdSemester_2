#include <sys/shm.h>
#include <unistd.h>

int
main(void)
{
    int key = ftok("/usr/", 'c');
    int id = shmget(key, 2 * sizeof(int), IPC_CREAT | 0666);
    int *p = shmat(id, 0, 0);
    p[0] = -1;
    p[1] = -1;
    
    if (!fork()) {
        p[0] = getpid();
    } else {
        if (!fork()) {
            p[1] = getpid();
        } else {
            while (p[0] == -1 && p[1] == -1);
            printf("%d %d\n", p[0], p[1]);
            shmdt(p);
        }
    }

    return 0;
}