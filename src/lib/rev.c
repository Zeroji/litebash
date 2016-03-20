#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LIST("rev")

DEF(rev)
{
    char * buffer;
    size_t n=0;
    while(!feof(stdin))
    {
        __ssize_t result = getline(&buffer, &n, stdin);
        if(result>0 && buffer[0]!='\n') {
            size_t i;
            for(i=strlen(buffer)-1; i--;)
                /*f*/putc(buffer[i], stdout);
            /*f*/putc('\n', stdout);
        }
    }
    free(buffer);
    return 0;
}
