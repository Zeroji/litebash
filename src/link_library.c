#define UNUSED(x) x __attribute__ ((unused))
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "link.h"
#include "lib/lib.h"

int init() { return 0; }

command getfunc(char const UNUSED(*name)) {
    // TODO
    return NULL;
}
