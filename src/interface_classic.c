#define UNUSED(x) x __attribute__ ((unused))
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "interface.h"
#include "lib/lib.h"

// Storing executable path
char exec_path[1024];
size_t exec_path_len;
int load_commands(char UNUSED(*dir_name)) {
    if(!getcwd(exec_path, 1024)) {
        printf("Error getting current working directory\n");
        return -1;
    }
    exec_path_len = strlen(exec_path);
    exec_path[exec_path_len++] = '/';
    return 0;
}
void unload_commands() { }

int _exec(int argc, char *argv[]);

Command getfunc(char *name) {
    strcpy(exec_path+exec_path_len, name);
    if(access(exec_path, X_OK) == -1) // Checking file existence & X permissions
        return exec_func;
    return _exec;
}

// Execute ./argv[0]
int _exec(int argc __attribute__((unused)), char *argv[]) {
    strcpy(exec_path+exec_path_len, argv[0]);
    argv[0]=exec_path;
    return execv(argv[0], argv);
}
