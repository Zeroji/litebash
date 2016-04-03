#define _POSIX_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "interface.h"

// execvp wrapper
int exec_func(int argc __attribute__((unused)), char *argv[]) {
    return execvp(argv[0], argv);
}

// Call a function with given arguments and streams, possibly asynchronously
int call(Command func, int argc, char *argv[], FILE* in, FILE* out, FILE* err, int async) {
    int status;
    int pid = fork();
    if(pid==-1) return -1;
    if(pid)
    {
        // Returning pid if asynchronously started, otherwise waiting for it
        if(async) return pid;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status))
            status = WEXITSTATUS(status);
    } else {
        // Replace standard streams by specified streams
        dup2(fileno(in), 0); dup2(fileno(out), 1); dup2(fileno(err), 2);
        exit(func(argc, argv));
    }
    return status;
}
