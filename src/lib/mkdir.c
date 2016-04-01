#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//create a directory if it doesn't exist in the current file

LIST("my_Mkdir")

DEF(my_Mkdir){
	struct stat status ={0};
	if (stat(nameDir, &status) == -1){
	mkdir(nameDir, 0700); //number for the right of the directory
    	return(0);
  	}
}
