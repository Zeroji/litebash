#include <stdlib.h>
#include "interface_struct.h"

// Implementation using a tree. Basically, to get the pointer to "cat" we go
// to TREE['c']['a']['t']['\0'] which is faster than looping on an array.

void *TREE[27];
int EMPTY = 1;

/* IMPORTANT NOTICE

This implementation assumes that the names are strings of lowercase letters
Any other character will be considered as the end of the string
*/

int add_elem_tree(char *name, void *elem, void *tree[])
{
    char c = name[0];
    if(c<'a' || c>'z')
    {
        if(tree[26] != NULL) return -1;
        tree[26] = elem;
        return (EMPTY=0);
    } // else
    int i = c - 'a';
    if(tree[i]==NULL)
        if(!(tree[i]=malloc(sizeof(void*)*27)))
            return 1;
    return add_elem_tree(name+1, elem, tree[i]);
}

int add_elem(char *name, void *elem) {
    return add_elem_tree(name, elem, TREE);
}

int is_empty() { return EMPTY; }

void* get_elem_tree(char *name, void* tree[])
{
    if(!tree) return NULL;
    char c = name[0];
    if(c<'a' || c>'z')
        return tree[26];
    return get_elem_tree(name+1, tree[c-'a']);
}

void* get_elem(char *name) {
    return get_elem_tree(name, TREE);
}

void delete_tree(void* tree[]) {
    size_t i;
    for (i = 0; i < 26; i++)
        if(tree[i])
            delete_tree(tree[i]);
    if(tree != TREE) // Root isn't dynamically allocated
        free(tree);
}

void delete_struct() {
    delete_tree(TREE);
}
