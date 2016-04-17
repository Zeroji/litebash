#include <string.h>
#include "interface.h"
#include "interface_struct.h"
#include "lib/lib.h"
#define STR(x) #x
#define ADD(x) add_elem(STR(x), x);

int load_commands(char *dir_name __attribute__((unused))) {
    // Add your commands here to have them available in static mode
    ADD(cat);
    ADD(chmod_lb);
    ADD(cp);
    ADD(echo);
    ADD(head);
    ADD(ls);
    ADD(mkdir_lb);
    ADD(mv);
    ADD(pwd);
    ADD(rev);
    ADD(rm);
    ADD(chown_lb);
    ADD(connect_lb);
    ADD(serveur_lb);
    return is_empty();
}

void unload_commands() {
    delete_struct();
}

Command getfunc(char *name) {
    Command func = (Command)get_elem(name);
    if(!func) return exec_func;
    return func;
}
