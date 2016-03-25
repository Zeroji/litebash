#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <stdio.h>

// Command typedef
typedef int (*Command)(int, char *[]);

// Load commands from directory & free them
int load_commands(char *dir_name);
void unload_commands();

// Get a function by its name
Command getfunc(char *name);

// execvp wrapper
int exec_func(int argc, char *argv[]);

// Call a function with given arguments and streams, possibly asynchronously
int call(Command func, int argc, char *argv[], FILE* in, FILE* out, FILE* err, int async);

#endif /* _INTERFACE_H */
