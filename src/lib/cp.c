#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

//copy a file into an other
LIST("cp")

DEF(cp){
	int samePath = 0;
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if(strcmp(cwd,argv[3]) == 0){
		samePath = 1;
	}
		FILE *fileToCopy, *fileCopy;

		fileToCopy = fopen(argv[1], "r");
		if(fileToCopy == NULL){
			exit(-1);
		}
		
		if (samePath){
			if(strcmp(argv[2], argv[1]) == 0){
				exit(-1);
			}
			else{
				fileCopy = fopen(argv[2], "w");
				if(fileCopy == NULL){
					exit(-1);
				}
				char c;
				while(!feof(fileToCopy)){
					c=fgetc(fileToCopy);
					fputc(c, fileCopy);
			}
		}
		else{
			chdir(argv[3]);
			fileCopy = fopen(argv[2], "w");
			if(fileCopy == NULL){
				exit(-1);
			}
				char c;
				while(!feof(fileToCopy)){
					c=fgetc(fileToCopy);
					fputc(c, fileCopy);
		}
		fclose(fileToCopy);
		fclose(fileCopy);
}


