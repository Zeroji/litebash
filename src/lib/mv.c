#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

//pove or rename a file
LIST("mv")

DEF(mv){
    int samePath = 0;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    if(strcmp(cwd,argv[3]) == 0){
        samePath = 1;
    }
        FILE *fileToCopy, *fileCopy, *fileToDelete;
	fileToDelete = argv[1];
        fileToCopy = fopen(argv[1], "r");
        if(fileToCopy == NULL){
            return(-1);
        }

        if (samePath){//problem segmentation fault but work
            if(strcmp(argv[2], argv[1]) == 0){
                return(-1);
            }
            else{
		int ren = rename(argv[1], argv[2]);
		if (ren == 0){
			return 0;
		} else{
			return -1;
		}
            }
        }
        else{//not same path change working directory
            chdir(argv[3]);
            fileCopy = fopen(argv[2], "w");
            if(fileCopy == NULL){
                return(-1);
            }
                int c;
                while(!feof(fileToCopy)){
                    c=fgetc(fileToCopy);
                    fputc(c, fileCopy);
                } 
        }
        fclose(fileToCopy);
        fclose(fileCopy);
	chdir(cwd);//to delete the old file, change working directory
	int res = remove(fileToDelete);
	if (res==0){
        	return 0; 
	}else
		return(-1);
}
