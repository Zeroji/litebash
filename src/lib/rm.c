#include <unistd.h>
#include <stdio.h>

//Delete a file 
//option not implement yet

LIST("rm")

DEF(rm){
	char filename[] = argv[1];
	if (fopen(filename, "r"))
		remove(filename);
	return(0);	
}
