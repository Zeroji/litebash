#ifndef _LIB_H
#define _LIB_H

// Add commands into this file. Please leave a short description.

// `cat` function. Copies stdin to stdout.
int cat(int argc, char *argv[]);

// `chmod` function. Changes files rights.
int chmod_lb(int argc, char *argv[]);

// chown function. Change the owner and the group owner of a file.
int chown_lb(int argc, char *argv[]);

// `cp` function. Copy a file into an other.
int cp(int argc, char *argv[]);

// `mv` function. move or rename a file
int mv(int argc, char *argv[]);

// `echo` function. Prints arguments.
int echo(int argc, char *argv[]);

// 'head' function. Prints the first 10 lines of a file.
int head(int argc, char*argv[]);

// `ls` function. List files and directories.
int ls(int argc, char *argv[]);

// `mkdir` function. Create a directory.
int mkdir_lb(int argc, char *argv[]);

// `pwd` function. Print the path of the current directory.
int pwd(int argc, char *argv[]);

// `rev` function. Reverse stdin to stdout.
int rev(int argc, char *argv[]);

// `rm` function. Remove file.
int rm(int argc, char *argv[]);

// `connect_lb` function. Connect to a server to execute remote commands.
int connect_lb(int argc, char *argv[]);

// `serveur_lb` function. Create a server.
int serveur_lb(int argc, char *argv[]);

#endif /* _LIB_H */
