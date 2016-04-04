#include <unistd.h>
#include <stdio.h>

//returns the canonical path for the current directory, without symlink and print it

LIST("pwd")

DEF(pwd){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    return(0);
}
