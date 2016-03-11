# litebash

### A note for outsiders

This is a school project made by four French students for class, whose goal is to create some kind of bash-like shell. If you accidentally stumbled across this project, don't look here, it's not meant to be useful code (nor readable code). Feel free to help, however, but we might not be writing all of it in English.

### About the command interface

My current idea is to have all commands use this prototype:

```c
int function(int argc, char const *argv[], FILE* in, FILE* out, FILE* err);
```

As usual, we have `argc` as the number of arguments, and `argv` as said arguments - we would still have `argv[0]` containing the "program" name, for example `ls`.  
The following three parameters would be file descriptors to be used as streams, like `stdin`, `stdout` and `stderr`. To use those for output, simply write `fprintf(out, ...)` instead of `printf(...)`, and for input I advise using `char c = fgetc(in)` (because `scanf` is bad). You can use `c == EOF`.  
Finally, the `int` return type is simply a standard error code - just `return 0;` if everything works fine.

### Code syntax

**Use spaces instead of tabs** - except for the Makefile of course. I'd recommend 4 spaces. Also, please don't put newlines before opening a curly bracket block, write like this:

```c
int main() {
    return 0;
}
```

Have at least one comment per function/block.
