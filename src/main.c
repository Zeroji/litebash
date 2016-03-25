#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "interface.h"

char path[4096];

int cd(char *name);
const char *delim = " ";

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

    // Searching for libraries in `pwd`/lib/
    char lib_path[1024];
    getcwd(lib_path, 1024);
    strcpy(lib_path + strlen(lib_path), "/lib/"),
    load_commands(lib_path);

    getcwd(path, 4096);

    // Command input (or taking from arguments)
    char *line=NULL;

    while(1) {
        do {
            free(line);
            if(!(line = readline("lb $ ")))
                break;
        } while(!strlen(line));
        if(!line) { printf("exit\n"); break; }

        char *name = strtok(line, delim);

        if(!strcmp(name, "exit")) {
            break;
        } else if(!strcmp(name, "cd")) {
            cd(strtok(NULL, delim));
        } else {
            Command func=getfunc(name);
            char *fargv[1024];
            if((fargv[0]=name)) {
                int n=0;
                while(n<1024 && (fargv[++n]=strtok(NULL, delim)));
                int fargc = n;
                int result = call(func, fargc, fargv, stdin, stdout, stderr, 0);
                if(result) printf("Execution error\n");
            }
        }
    }

    free(line);
    unload_commands();
    return 0;
}

int cd(char *name) {
    if(chdir(name)) {
        if(errno == EACCES)
            fprintf(stderr, "lb: cd: %s: Permission denied\n", name);
        if(errno == ENOENT)
            fprintf(stderr, "lb: cd: %s: No such file or directory\n", name);
        return -1;
    }
    return 0;
}
