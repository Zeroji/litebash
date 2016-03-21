#include <string.h>
#include "interface.h"
#include "interface_struct.h"
#include "lib/lib.h"
#define STR(x) #x
#define ADD(x) add_elem(STR(x), x);

int load_commands(char *dir_name __attribute__((unused))) {
    // Add your commands here to have them available in static mode
    ADD(cat);
    ADD(rev);
    ADD(ls);
    return is_empty();
}

void unload_commands() {
    delete_struct();
}

Command getfunc(char *name) {
    return (Command)get_elem(name);
}