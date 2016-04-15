#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

LIST("rev")

DEF(rev)
{
    void rev_print(FILE* data) {
        char * buffer;
        size_t n=0;
        while(!feof(data))
        {
            __ssize_t result = getline(&buffer, &n, data);
            if(result>0 && buffer[0]!='\n') {
                size_t i;
                for(i=strlen(buffer)-1; i--;)
                    printf("%c", buffer[i]);
                printf("\n");
            }
        }
        free(buffer);
    }

    if(argc == 1)
        rev_print(stdin);
    else {
        int ind = 1;
        for(;  ind < argc; ind++) {
            char *name = argv[ind];
            if(access(name, F_OK))
                fprintf(stderr, "rev: %s: no such file or directory\n", name);
            else if(access(name, R_OK))
                fprintf(stderr, "rev: %s: permission denied\n", name);
            else {
                FILE* file = fopen(name, "r");
                if(!file) fprintf(stderr, "cat: %s: opening error\n", name);
                else rev_print(file);
            }
        }
    }
    exit(EXIT_SUCCESS);
}
