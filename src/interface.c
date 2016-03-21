#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include "interface.h"

// Call a function with given arguments and streams, possibly asynchronously
int call(Command func, int argc, char *argv[], FILE* in, FILE* out, FILE* err, int async) {
    int result;
    int *res=&result;
    int pid = fork();
    if(pid==-1) return -1;
    if(pid)
    {
        // Returning pid if asynchronously started, otherwise waiting for it
        if(async) return pid;
        wait(NULL);
    } else {
        // Replace standard streams by specified streams
        dup2(fileno(in), 0); dup2(fileno(out), 1); dup2(fileno(err), 2);
        *res = func(argc, argv);
    }
    return result;
}
