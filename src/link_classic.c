#define UNUSED(x) x __attribute__ ((unused))
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "link.h"
#include "lib/lib.h"

int init() { return 0; }

int _exec(int argc, char *argv[]);

command getfunc(char const *name) {
    if(access(name, X_OK) == -1)
        return NULL;
    return _exec;
}

int _exec(int UNUSED(argc), char *argv[]) {
    char path[1024] = "./";
    strcpy(path+2, argv[0]);
    argv[0]=path;
    int pid=fork();
    if(pid==-1) return -1;
    if(pid)
    {
        int status;
        waitpid(pid, &status, 0);
        return status;
    } else {
        execvp(argv[0], argv);
    }
    return -1;
}
