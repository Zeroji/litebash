#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include "link.h"

int call(command func, int argc, char *argv[], FILE* in, FILE* out, FILE* err, int async) {
    int pid = fork();
    if(pid==-1) return -1;
    int result;
    int *res=&result;
    if(pid)
    {
        if(async) return pid;
        wait(NULL);
    } else {
        // int copy[3];
        // for(int i=0; i<3; i++)
        //     copy[i] = dup(i);

        dup2(fileno(in), 0); dup2(fileno(out), 1); dup2(fileno(err), 2);

        *res = func(argc, argv);

        // for(int i=0; i<3; i++) {
        //     dup2(copy[i], i);
        //     close(copy[i]);
        // }
    }
    return result;
}
