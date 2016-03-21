#ifndef _INTERFACE_STRUCT_H
#define _INTERFACE_STRUCT_H

#include "interface.h"

// Structure to store function pointers by their names
// Actually a dictionary able to contain any kind of element
// Names should be NULL-terminated lowercase strings

int add_elem(char *name, void *elem);

void* get_elem(char *name);

int is_empty();

void delete_struct();

#endif /* _INTERFACE_STRUCT_H */
