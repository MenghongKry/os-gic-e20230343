/* forkchild.c — Process creation with fork() and exec() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    /*
     * Line-buffer stdout. When output is redirected to a file, stdout is
     * fully buffered: the parent's first line would be copied into the child
     * by fork() (printed twice), and the child's lines would be thrown away
     * by execlp(). Line buffering writes each line out immediately.
     */
    setvbuf(stdout, NULL, _IOLBF, 0);

    printf("Parent process (PID: %d) — creating child...\n", getpid());

    pid = fork();

    if (pid < 0) {
        /* fork failed */
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        /* ---- CHILD PROCESS ---- */
        printf("Child process (PID: %d, Parent PID: %d)\n", getpid(), getppid());

        /*
         * Sleep for 30 seconds so you have time to observe
         * the parent-child process tree in another terminal
         * using ps, top, or htop (see step 5 below).
         */
        printf("Child: sleeping 30s — observe the process tree now!\n");
        sleep(30);

        printf("Child: now executing 'ls -la'...\n");

        /* Replace this process with "ls -la" */
        execlp("ls", "ls", "-la", NULL);

        /* If execlp returns, it means it failed */
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else {
        /* ---- PARENT PROCESS ---- */
        int status;
        printf("Parent: waiting for child (PID: %d) to finish...\n", pid);

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Parent: child exited with status %d\n", WEXITSTATUS(status));
        }
        printf("Parent: done.\n");
    }

    return 0;
}
