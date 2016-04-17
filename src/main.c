#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>
#include "interface.h"
#include "parser.h"

char path[4096];

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

    // Searching for libraries in `pwd`/lib/
    char lib_path[1024];
    getcwd(lib_path, 1024);
    sprintf(lib_path + strlen(lib_path), "/%s/%s", dirname(argv[0]), "lib/");
    if(load_commands(lib_path)) return EXIT_FAILURE;
    // Unloading commands upon exit
    atexit(unload_commands);

    getcwd(path, 4096);

    // Command input (or taking from arguments)
    char *line=NULL;
    char prompt[4096];

    while(1) {
        do {
            free(line);
            getcwd(path, 4096);
            sprintf(prompt, "lb %s $ ", path);
            if(!(line = readline(prompt)))
                break;
        } while(!strlen(line));
        if(!line) { printf("exit\n"); break; }

        parse(line, stdin, stdout, stderr);
    }

    free(line);
    return 0;
}
