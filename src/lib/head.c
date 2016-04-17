#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

LIST("head");

DEF(head) {
    int byte_mode = 0;
    long lines = 10;
    int verbosity = 1;

    void head_print(FILE* data, char* name) {
        if(verbosity)
            printf("==> %s <==\n", name);
        int iter = 0;
        while(iter != lines && !feof(data)) {
            int c = fgetc(data);
            if(c == '\n' || byte_mode) iter++;
            printf("%c", c);
        }
    }

    char *argerr;
    char *args = "c:n:qv";
    int c;

    while((c=getopt(argc, argv, args))+1) {
        switch(c) {
            case 'c':
                lines = strtol(optarg, &argerr, 10);
                if(*argerr) {
                    fprintf(stderr, "head: %s: invalid number of bytes\n", optarg);
                    exit(EXIT_FAILURE);
                }
                byte_mode = 1;
                break;
            case 'n':
                lines = strtol(optarg, &argerr, 10);
                if(*argerr) {
                    fprintf(stderr, "head: %s: invalid number of lines\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'q':
                verbosity = 0;
                break;
            case 'v':
                verbosity = 2;
                break;
        }
    }

    if(lines < 0) {
        printf("head doesn't handle negative lines (yet)\n");
        return -1;
    }

    if(verbosity == 1)
        verbosity = optind + 1 < argc;

    if(optind < argc) {
        for(; optind < argc;) {
            char *name = argv[optind];
            if(!strcmp(name, "-"))
                head_print(stdin, "standard input");
            else if(access(name, F_OK))
                fprintf(stderr, "cat: %s: no such file or directory\n", name);
            else if(access(name, R_OK))
                fprintf(stderr, "cat: %s: permission denied\n", name);
            else {
                FILE* file = fopen(name, "r");
                if(!file) fprintf(stderr, "cat: %s: opening error\n", name);
                else head_print(file, name);
            }
            if(++optind != argc && verbosity)
                printf("\n");
        }
    } else { head_print(stdin, "standard input"); }
    return 0;
}
