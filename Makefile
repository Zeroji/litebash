CFLAGS	= -Wall -Werror -std=c99 -g -O0
CLINKS	= -lm -ldl -lreadline
LIBSRC	= $(wildcard src/lib/*.c)
LIBOBJ	= $(LIBSRC:src/lib/%.c=obj/%.o)

# Defining macros for library files
# Trust me, I'm an engineer
_CLASSIC = -D DEF\(func\)=int\ main\(int\ argc\ __attribute\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) -D LIST\(...\)= \
	   -D DEF_ADD\(func\)=int\ func\(int\ argc\ __attribute\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\)
_STATIC  = -D DEF\(func\)=int\ func\(int\ argc\ __attribute\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) \
           -D DEF_ADD\(x\)=DEF\(x\) -D LIST\(...\)= -include src/lib/lib.h
_LIBRARY = -D DEF\(func\)=int\ func\(int\ argc\ __attribute\(\(unused\)\),\ char\ *argv[]\ __attribute__\(\(unused\)\)\) \
           -D DEF_ADD\(x\)=DEF\(x\) -D LIST\(...\)=char\ \*_FL\[\]=\{__VA_ARGS__,\ NULL\}\;

CC		= gcc $(CFLAGS)

CLASSIC = litebash
STATIC  = bash_static
LIBRARY = bash_dyn

all: $(CLASSIC)

%: src/misc/%.c
	$(CC) -o $@ $< $(CLINKS)

# Classic build (separate executables)
$(CLASSIC): src/main.c src/link_classic.c src/link.c
	$(CC) -o $@ $^ $(CLINKS)

%: src/lib/%.c
	$(CC) -o $@ $< $(_CLASSIC) $(CLINKS)

# Standalone build (static)
$(STATIC): src/main.c src/link_static.c src/link.c $(LIBOBJ)
	$(CC) -o $@ $^ $(CLINKS)

obj/%.o: src/lib/%.c
	$(CC) -o $@ -c $< $(_STATIC) $(CLINKS)

# Shared library build
$(LIBRARY): src/main.c src/link_library.c src/link.c
	$(CC) -o $@ $^ $(CLINKS)

obj/lib/%.o: src/lib/%.c
	$(CC) -o $@ -c $< -fPIC $(CLINKS) $(_LIBRARY)

%.so: obj/lib/%.o
	$(CC) -shared -Wl,-soname,$@ $< -o lib/$@

# Using -f so it won't error if there are no files
clean:
	rm -f obj/*.o
	rm -f obj/lib/*.o

# Might be useful.
cleanlibs:
	rm -f lib/*.so
