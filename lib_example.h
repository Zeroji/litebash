#ifndef _LIB_H
#define _LIB_H
#include <stdio.h>  // For FILE*

// `cat` function. Copies stdin to stdout.
int cat(int argc, char const *argv[], FILE* in, FILE* out, FILE* err);

#endif
