#include <stdio.h>

LIST("echo")

DEF(echo) {
  int i;
  for (i=1; i<argc; i++) {
    printf("%s ",argv[i]);
  }
  printf("\n");
  return 0;
}
