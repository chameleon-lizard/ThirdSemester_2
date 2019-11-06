#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <wait.h>

//  (pr1 arg1 > f) && (pr2 | pr3 args...)

enum
{
    PR1 = 1,
    ARG1 = 2,
    F = 3,
    PR2 = 4,
    PR3 = 5,
    ARGS = 6
};

int
main(int argc, char *argv[])
{
    if (argc < ARGS) {
        return 1;
    }

    if (fork() == 0) {
        // Here we created a child process, it uses pr1 with arg1 and writes it to file f.
        int f = open(argv[F], O_CREAT | O_WRONLY | O_TRUNC, 0777);
        close(1);
        dup(f);
        close(f);
        execlp(argv[PR1], argv[PR1], argv[ARG1], NULL);
        _exit(1);
    } else {
        int status = 0;
        wait(&status);
        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
            // This means that everything returned success and we can continue to the second partю
            // We are now creating a pipe to ensure communication between processes.
            int communication[2];
            pipe(communication);
            if (fork() == 0) {
                // Here we have a second child; we waited for the first one to die, so this is the
                // only sibling of the original parent process. We use dup2 to duplicate a file
                // descriptor from stdout to stdin, so we basically have a pipe from the child to
                // it's brother.
                close(communication[0]);
                dup2(communication[1], STDOUT_FILENO);
                close(communication[1]);
                execlp(argv[PR2], argv[PR2], NULL);
                _exit(1);
            } else {
                wait(&status);
                if (WIFEXITED(status) && !WEXITSTATUS(status)) {
                    if (fork() == 0) {
                        // Created a child again, so we can ensure that everything went smoothly
                        // by checking exit code in father's process.
                        close(communication[1]);
                        dup2(communication[0], STDIN_FILENO);
                        close(communication[0]);
                        execlp(argv[PR3], argv[PR3], argv[ARGS], NULL);
                        _exit(1);
                    } else {
                        //wait(&status);
                        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
                            printf("This is the end, my only friend.\n");
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}