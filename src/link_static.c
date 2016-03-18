#include <string.h>
#include "link.h"
#include "lib/lib.h"

command FUNCS[] = {cat, rev};
const char *NAMES[] = {"cat", "rev"};
const size_t FSIZE = 2;

int init() { return !FSIZE; }

//int (*getfunc(char const *name))(int, char const*[]/*, FILE*, FILE*, FILE**/) {
command getfunc(char const *name) {
    size_t i;
    for (i = 0; i < FSIZE; i++)
        if(!strcmp(NAMES[i], name))
            return FUNCS[i];
    return NULL;
}
