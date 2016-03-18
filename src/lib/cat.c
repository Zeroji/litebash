#include <stdio.h>

LIST("cat")

DEF(cat)
{
    int c;
    while((c=fgetc(stdin))!=EOF)
        printf("%c", c);
    return 0;
}
