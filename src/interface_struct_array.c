#include <string.h>
#include "interface_struct.h"
#define F_MAX_SIZE 1024

// Implementation with fixed-size array (first idea)

char *F_NAMES[F_MAX_SIZE];
void *F_FUNCS[F_MAX_SIZE];
size_t F_SIZE = 0;

int add_elem(char *name, void *elem)
{
    if(F_SIZE == F_MAX_SIZE)
        return 1;
    size_t i;
    for (i = 0; i < F_SIZE; i++)
        if(!strcmp(F_NAMES[i], name))
            return -1;
    F_NAMES[F_SIZE] = name;
    F_FUNCS[F_SIZE] = elem;
    F_SIZE++;
    return 0;
}

int is_empty() { return !F_SIZE; }

// Doing strcmp on every name in the array...
void* get_elem(char *name)
{
    size_t i;
    for (i = 0; i < F_SIZE; i++)
        if(!strcmp(F_NAMES[i], name))
            return F_FUNCS[i];
    return NULL;
}

void delete_struct() {}
