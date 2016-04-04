#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//create a directory if it doesn't exist in the current file

LIST("mkdir_lb")

DEF(mkdir_lb){
    char *nameDir = argv[1]; // @PHippolyte // Genre t'as oublié ça
    struct stat status ={0};
    if (stat(nameDir, &status) == -1){
    mkdir(nameDir, 0700); //number for the right of the directory
        return(0);
    }
    return -1; // @PHippolyte // Toujours return en fin de fonction
}
