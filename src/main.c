#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "link.h"
#define UNUSED(x) x __attribute__ ((unused))

int main(int argc, char *argv[]) {
    char *line;
    if(argc > 1) {
        line = argv[1];
    } else {
        do {
            line = readline("lb $ ");
        } while(!strlen(line));
    }

    command func = getfunc(line);
    if(!func) {
        fprintf(stderr, "%s: command not found\n", line);
        return -1;
    }
    int fargc = 1;
    char *fargv[2];
    fargv[0] = line;
    fargv[1] = NULL;
    int result = call(func, fargc, fargv, stdin, stdout, stderr, 0);
    return result;
}
