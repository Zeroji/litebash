# How to write your commands

The Makefile is quite messy, but that allows you to write less stuff in your command files.

### Command files

They should be located in `src/lib`. Compiled object files will go in `obj`, or in `obj/lib` when compiled for shared libraries. Compiled libraries will go in `lib`.

### Writing a command

To declare a command, use the macro `DEF(name)`. It'll include `argc` and `argv` so you won't have to worry about those. I recommend having only one command per file, however if you need more then please use `DEF_ADD(name)` for the other commands.  
Each file should contain a line listing the commands with `LIST(...)`.

A short example:
```c
LIST("cat", "more", "less"); // Add commands to list (`;` is optional)

DEF(cat) { ... } // Define main command
DEF_ADD(more) { ... } // Define additional command
DEF_ADD(less) { ... } // Define additional command
```

### Adding a command for static mode

In order for the static compilation to include your command, you will have to edit the following files:  
- Add one line in `src/interface_static.c`

```c
int load_commands(char *dir_name __attribute__((unused))) {
    // Add your commands here to have them available in static mode
    ADD(cat);
    ADD(ls);
    return is_empty();
}
```

- Add some more in `src/lib/lib.h`

```c
#ifndef _LIB_H
#define _LIB_H

// Add commands into this file. Please leave a short description.

// `cat` function. Copies stdin to stdout.
int cat(int argc, char *argv[]);

// `ls` function. List files and directories.
int ls(int argc, char *argv[]);

#endif /* _LIB_H */
```
