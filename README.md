# litebash

### A note for outsiders

This is a school project made by four French students for class, whose goal is to create some kind of bash-like shell. If you accidentally stumbled across this project, don't look here, it's not meant to be useful code (nor readable code). Feel free to help, however, but we might not be writing all of it in English.

### About the command interface

*Detailed explanation (in French) is available in [doc/interface.md](https://github.com/Zeroji/litebash/blob/master/doc/interface.md).*  
*To command developers, please read [doc/commands.md](https://github.com/Zeroji/litebash/blob/master/doc/commands.md).*

We will simply use this:

```
int function(int argc, char const *argv[]);
```

As usual, we have `argc` as the number of arguments, and `argv` as said arguments - we would still have `argv[0]` containing the "program" name, for example `ls`.  
The interpreter will handle streams and piping, so you can use `printf` and `scanf` as usual - though `char c = fgetc(stdin)` is much better than `scanf`.  
Finally, the `int` return type is simply a standard error code - just `return 0;` if everything works fine.

### Code syntax

**Use spaces instead of tabs** - except for the Makefile of course. I'd recommend 4 spaces.  
Have at least one comment per function/block.

### Using the Makefile

This section was moved to [doc/makefile.md](https://github.com/Zeroji/litebash/blob/master/doc/makefile.md).
