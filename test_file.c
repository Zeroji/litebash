#include <stdlib.h>
#include <stdio.h>
#include "lib_example.h"

int main(int argc, char const *argv[]) {
    argv[0] = "cat";  // Function name here
    return cat(argc, argv, stdin, stdout, stderr);
}
