# Using the Makefile

### Compilation modes

The Makefile defines the following names:
```
CLASSIC = litebash
STATIC  = bash_static
DYNAMIC = bash_dyn
```
By default, `make` and `make all` build the classic version of the code, you can use `make bash_static` or `make bash_dyn` for the other two.

### Commands for the commmands

You can use the following `make` commands:  
- `make file` will attempt to build the program (`cat`,...) `src/lib/file.c` or the program `src/misc/file.c`
- `make file.so` will attempt to build the shared library `src/lib/file.c` into `lib/file.so`
- `make clean` will remove the `.o` files
- `make cleanlibs` will remove the `.so` files

### Editing the Makefile

You might need to add a linker option (like `-ldl`) or a gcc flag, in that case feel free to edit the first lines of the file to change the variables `CFLAGS`, `CLINKS` and `LLINKS`.  
You can also change the executable names if you want by modifying `CLASSIC`, `STATIC` and `DYNAMIC`. Note that this will change the commands.

Avoid pushing a modified Makefile to git without a good reason.

### Usage examples

- Classic compilation:

```
make
make cat ls
./litebash
```
This will build `litebash`, then the commands `cat` and `rev`, and run it.

- Static compilation:

```
make bash_static
```

- Dynamic compilation:

```
make bash_dyn
make cat.so ls.so
```
You'll have to build each library, otherwise it won't work.
