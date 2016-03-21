#define UNUSED(x) x __attribute__ ((unused))
#include <dirent.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "interface_struct.h"

// Small linked list to get a heap
typedef struct SCell {
    struct SCell *next;
    void* lib;
} SCell;

typedef struct SList {
    SCell *head;
} SList;

void add_lib(SList *list, void* lib) {
    SCell *cell = malloc(sizeof(SCell));
    cell -> lib = lib;
    cell -> next = list -> head;
    list -> head = cell;
}

// Deleting the list closes the libraries
void free_libs(SList *list) {
    SCell *cell = list -> head;
    while(cell) {
        dlclose(cell -> lib);
        SCell *next = cell -> next;
        free(cell);
        cell = next;
    }
    list -> head = NULL;
}

// Open libraries list
SList F_LIBS;

int load_commands(char *dir_name) {
    F_LIBS.head=NULL;
    char path[1024];
    strcpy(path, dir_name);
    size_t path_len = strlen(path);
    DIR *dir;
    if(!(dir = opendir(dir_name))) { // Open directory
        printf("Failed to open directory: %s\n", dir_name);
        return -1;
    }
    struct dirent *lib;
    int no_libs=1;
    int error=0;
    while((lib = readdir(dir)) && !error) {
        size_t lib_len = strlen(lib->d_name); // Get the .so files
        if(lib_len>3 && !strcmp(lib->d_name+lib_len-3, ".so")) {
            strcpy(path + path_len, lib->d_name);
            void *library;
            if (!(library = dlopen (path, RTLD_LAZY))) { // Open the library
                printf ("Failed to open library: %s\n", dlerror ());
                error=1; break;
            }
            add_lib(&F_LIBS, library);
            char **functions;
            if(!(functions = dlsym(library, "_FL"))) { // Get function list
                printf ("Failed to get function list: %s\n", dlerror ());
                error=2; break;
            }
            int i=0;
            while(functions[i]) {
                char *name=functions[i++];
                Command func;
                if(!(func=dlsym(library, name))) { // Get each function
                    printf("Failed to get function %s: %s\n", name, dlerror());
                    error=2; break;
                }
                no_libs = 0;
                add_elem(name, func);
            }
        }
    }
    closedir(dir);
    if(error) {
        free_libs(&F_LIBS);
        return error;
    }
    if(no_libs) {
        printf("No libraries found in %s.\n", dir_name);
        return -1;
    }
    return 0;
}

void unload_commands() {
    // Close the libraries
    free_libs(&F_LIBS);
    // Properly delete the structure
    delete_struct();
}

Command getfunc(char *name) {
    return get_elem(name);
}
