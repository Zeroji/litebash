#ifndef _LIB_H
#define _LIB_H

// Add commands into this file. Please leave a short description.

// `cat` function. Copies stdin to stdout.
int cat(int argc, char *argv[]);

// `rev` function. Reverse stdin to stdout.
int rev(int argc, char *argv[]);

// `ls` function. List files and directories.
int ls(int argc, char *argv[]);

// `pwd` function. Print the path of the current directory.
int pwd(int argc, char *argv[]);

//`mkdir` function. Create a directory.
int mkdir(int argc, char *argv[]);

//`rm` function. Remove file.
int rm(int argc, char *argv[]);

#endif /* _LIB_H */
