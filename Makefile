# Compiler flags
CFLAGS	= -Wall -Werror -g -O0
# Interpreter links
CLINKS	= -lm -ldl -lreadline
# Library links
LLINKS	=
# Compiler
CC		= gcc $(CFLAGS)
# Interpreter common files
CFILES = src/main.c src/interface.c src/parser.c

# Executable names
CLASSIC = litebash
STATIC  = bash_static
DYNAMIC = bash_dyn

# PLEASE AVOID EDITING UNDER THIS LINE #
LIBSRC	= $(wildcard src/lib/*.c)
LIBOBJ	= $(LIBSRC:src/lib/%.c=obj/%.o)

# Defining macros for library files
# Trust me, I'm an engineer
_CLASSIC = -D DEF\(func\)=int\ main\(int\ argc\ __attribute__\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) -D LIST\(...\)= \
	   -D DEF_ADD\(func\)=int\ func\(int\ argc\ __attribute__\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\)
_STATIC  = -D DEF\(func\)=int\ func\(int\ argc\ __attribute__\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) \
           -D DEF_ADD\(x\)=DEF\(x\) -D LIST\(...\)= -include src/lib/lib.h
_DYNAMIC = -D DEF\(func\)=int\ func\(int\ argc\ __attribute__\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) \
           -D DEF_ADD\(x\)=DEF\(x\) -D LIST\(...\)=char\ \*_FL\[\]=\{__VA_ARGS__,\ NULL\}\;

# Make all / misc
all: $(CLASSIC)

%: src/misc/%.c
	$(CC) -o $@ $< $(CLINKS)

# Make server
server: $(CFILES) src/interface_classic.c src/serveur_lb.c
	$(CC) -o $@ $^ $(CLINKS) $(_CLASSIC) -D _CLASSIC

# Classic build (separate executables)
$(CLASSIC): $(CFILES) src/interface_classic.c
	$(CC) -o $@ $^ $(CLINKS) -D _CLASSIC

%: src/lib/%.c
	$(CC) -o $@ $< $(_CLASSIC) $(LLINKS)

# Static build (standalone)
$(STATIC): $(CFILES) src/interface_static.c src/interface_struct_tree.c $(LIBOBJ)
	$(CC) -o $@ $^ $(CLINKS) -D _STATIC

obj/%.o: src/lib/%.c
	$(CC) -o $@ -c $< $(_STATIC) $(LLINKS)

# Dynamic build (shared libraries)
$(DYNAMIC): $(CFILES) src/interface_dynamic.c src/interface_struct_tree.c
	$(CC) -o $@ $^ $(CLINKS) -D _DYNAMIC

obj/lib/%.o: src/lib/%.c
	$(CC) -o $@ -c $< -fPIC $(LLINKS) $(_DYNAMIC)

%.so: obj/lib/%.o
	$(CC) -shared -Wl,-soname,$@ $< -o lib/$@

# Using -f so it won't error if there are no files
clean:
	rm -f obj/*.o
	rm -f obj/lib/*.o

# Might be useful.
cleanlibs:
	rm -f lib/*.so
