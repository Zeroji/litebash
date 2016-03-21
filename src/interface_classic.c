#define UNUSED(x) x __attribute__ ((unused))
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "interface.h"
#include "lib/lib.h"

// No need to do anything here
int load_commands(char UNUSED(*dir_name)) { return 0; }
void unload_commands() { }

int _exec(int argc, char *argv[]);

Command getfunc(char *name) {
    if(access(name, X_OK) == -1) // Checking file existence & X permissions
        return NULL;
    return _exec;
}

// Execute ./argv[0]
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
