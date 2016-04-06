#include <stdio.h>

LIST("echo")

DEF(echo) {
  int i;
  printf("%s",argv[1]);
  for (i=2; i<argc; i++) {
    printf(" %s",argv[i]);
  }
  printf("\n");
  return 0;
}
