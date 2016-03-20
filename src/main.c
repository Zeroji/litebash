#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "interface.h"

int main(int argc, char *argv[]) {

    // Searching for libraries in `pwd`/lib/
    char lib_path[1024];
    getcwd(lib_path, 1024);
    strcpy(lib_path + strlen(lib_path), "/lib/"),
    load_commands(lib_path);

    // Command input (or taking from arguments)
    char *line=NULL;
    if(argc > 1) {
        line = malloc(strlen(argv[1])+1);
        strcpy(line, argv[1]);
    } else {
        do {
            free(line);
            line = readline("lb $ ");
        } while(!strlen(line));
    }

    Command func = getfunc(line);
    if(!func) {
        fprintf(stderr, "%s: command not found\n", line);
        unload_commands();
        free(line);
        return -1;
    }
    int fargc = 1;
    char *fargv[2];
    fargv[0] = line;
    fargv[1] = NULL;
    int result = call(func, fargc, fargv, stdin, stdout, stderr, 0);
    free(line);
    unload_commands();
    return result;
}
