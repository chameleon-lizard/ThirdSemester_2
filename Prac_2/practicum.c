#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <wait.h>

//  (pr1 arg1 > f) && (pr2 | pr3 args...)

// This is the global variable, where there the pid of the child process is being stored.
int pid = 0;

enum
{
    PR1 = 1,
    ARG1 = 2,
    F = 3,
    PR2 = 4,
    PR3 = 5,
    ARGS = 6
};

void
fsig(int sig)
{
    // Sending signal to the only non-dead son :(

    kill(pid, sig);
    _exit(0);
}

int
main(int argc, char *argv[])
{
    // Checking if there are enough parameters.
    if (argc < ARGS) {
        printf("Not enough arguments specified.\n");
        return 1;
    }

    // Tieing SIGKILL to the signal function.
    signal(SIGKILL, fsig);

    if ((pid = fork()) == 0) {
        // Here we created a child process, it uses pr1 with arg1 and writes it to file f.
        int f = open(argv[F], O_CREAT | O_WRONLY | O_TRUNC, 0777);

        // Now we are duplicatung the descriptors of file to write into file instead of
        // stdout.
        close(STDOUT_FILENO);
        dup(f);
        close(f);

         sleep(5);

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

            if ((pid = fork()) == 0) {
                // Here we have a second child; we waited for the first one to die, so this is the
                // only sibling of the original parent process. We use dup2 to duplicate the descriptor.
                // Now we can write to the pipe.
                close(communication[0]);
                dup2(communication[1], STDOUT_FILENO);
                close(communication[1]);
                
                sleep(5);
                
                execlp(argv[PR2], argv[PR2], NULL);
                _exit(1);
            } else {
                wait(&status);
                if (WIFEXITED(status) && !WEXITSTATUS(status)) {
                    if ((pid = fork()) == 0) {
                        // Created a child again, so we can ensure that everything went smoothly
                        // by checking exit code in father's process. We also duplicate the descriptor;
                        // now we can use the pipe to read data that was sent from the brother 
                        // to the pipe.
                        close(communication[1]);
                        dup2(communication[0], STDIN_FILENO);
                        close(communication[0]);

                        sleep(5);

                        execvp(argv[PR3], &argv[PR3]);
                        _exit(1);
                    } else {
                        // Closing all pipes in father, so it won't freeze, waiting for a successful
                        // execution of children, exiting with success code.
                        close(communication[0]);
                        close(communication[1]);

                        wait(&status);
                        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
                            return 0;
                        } else {
                            printf("A problem occured during execution of the pr3.\n");
                            return 1;
                        }
                    }
                } else {
                            printf("A problem occured during execution of the pr2.\n");
                            return 1;
                }
            }
        } else {
            printf("A problem occured during execution of the pr1.\n");
            return 1;
        }
    }
    
    return 0;
}