#include "lib_example.h"

// `cat` function. Copies stdin to stdout.
int cat(int argc, char const *argv[], FILE* in, FILE* out, FILE* err) {
    int c;
    while((c=fgetc(in))!=EOF)
        fprintf(out, "%c", c);
    return 0;
}
