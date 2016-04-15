#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

LIST("cat")

DEF(cat)
{
    int number = 0, num_nonempty = 0, ends = 0, squeeze = 0, tabs = 0, nonprinting = 0;
    int lines = 0;

    void cat_print(FILE* data) {
        int c;
        int newline = 1, tab = 0;
        int empty;
        int double_newline = 0;
        int no_print = 0;
        char line_num_buffer[10];
        while((c = fgetc(data)) != EOF) {
            c &= 255;
            empty = newline && c=='\n';
            if(double_newline && empty && squeeze) continue;
            double_newline = newline && c == '\n';
            if(newline && number && (!num_nonempty || !empty)) {
                sprintf(line_num_buffer, "%d", ++lines);
                printf("%6.6s\t", line_num_buffer);
                newline = 0;
            }
            no_print = 0;
            newline = c == '\n';
            tab = c == '\t';
            if(ends && newline) printf("$");
            if(tabs && tab) {
                printf("^I");
                no_print = 1;
            }
            if(nonprinting && c > 127) {
                printf("M-");
                c &= 127;
            }
            if(nonprinting && !newline && !tab && c < 32) {
                printf("^%c", c+64);
                no_print = 1;
            }
            if(nonprinting && c == 127) {
                printf("^?");
                no_print = 1;
            }
            if(!no_print)
                printf("%c", c);
        }
    }

    char *args = "AbeEnstTuv";
    int c;

    while((c=getopt(argc, argv, args))+1) {
        switch(c) {
            case 'A': // -vET
                tabs = 1;
            case 'e': // -vE
                nonprinting = 1;
            case 'E': // -E
                ends = 1;
                break;
            case 'b': // -b overrides -n
                num_nonempty = 1;
            case 'n': // -n
                number = 1;
                break;
            case 's': // -s
                squeeze = 1;
                break;
            case 't': // -vT
                nonprinting = 1;
            case 'T': // -T
                tabs = 1;
                break;
            case 'u': // ignored
                break;
            case 'v':
                nonprinting = 1;
                break;
        }
    }

    if(optind < argc) {
        for(; optind < argc; optind++) {
            char *name = argv[optind];
            if(!strcmp(name, "-"))
                cat_print(stdin);
            else if(access(name, F_OK))
                fprintf(stderr, "cat: %s: no such file or directory\n", name);
            else if(access(name, R_OK))
                fprintf(stderr, "cat: %s: permission denied\n", name);
            else {
                FILE* file = fopen(name, "r");
                if(!file) fprintf(stderr, "cat: %s: opening error\n", name);
                else cat_print(file);
            }
        }
    } else { cat_print(stdin); }
    exit(EXIT_SUCCESS);
}
