// Program to test shared libraries
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc < 2) { // Oops
        printf("Usage: %s library_path.so [command_name [args]]\n", argv[0]);
        return -1;
    }

    // Get library path
    char path[1024];
    // Absolute path: easy
    if(argv[1][0] == '/')
        strcpy(path, argv[1]);
    else { // Relative: append CurrentWorkingDirectory
        getcwd(path, 1024);
        size_t len = strlen(path);
        path[len++] = '/';
        strcpy(path + len, argv[1]);
        // printf("Loading library from %s\n", path);
    }

    void *library;
    if (!(library = dlopen (path, RTLD_LAZY))) { // Oops
        printf ("dlopen error: %s\n", dlerror ());
        return -1;
    }
    if(argc > 2) { // If user specified a command to execute
        int (*func)(int, char*[])=NULL;
        if (!(func = dlsym (library, argv[2]))) { // Oops
            printf ("dlsym error: %s\n", dlerror ());
            dlclose (library);
            return -1;
        }
        func (argc-2, argv+2); // Call with function name + additional arguments
    } else { // Just get function list and print it
        char **list;
        if(!(list = dlsym(library, "_FL"))) { // Oops
            printf ("dlsym error: %s\n", dlerror ());
            dlclose (library);
            return -1;
        }
        int i=0;
        while(list[i]) { // Looping on the (NULL-terminated) list
            printf("FL[%d]: %s\n", i, list[i]);
            i++;
        }
    }
    dlclose (library); // Close the library properly when done
    return 0;
}
