#ifndef _LINK_H
#define _LINK_H

#include <stdio.h>

typedef int (*command)(int, char *[]);

int call(command func, int argc, char *argv[], FILE* in, FILE* out, FILE* err, int async);

command getfunc(char const *name);
//int (*getfunc(char const *name))(int, char const*[]/*, FILE*, FILE*, FILE**/);

void addff(char const *name, command func);

#endif /* _LINK_H */
