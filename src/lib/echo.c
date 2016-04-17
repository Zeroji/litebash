#include <stdio.h>

LIST("echo")

DEF(echo) {
  int i;
  // Pour chaque arguments donnés, on leur fait un printf
  printf("%s",argv[1]);
  for (i=2; i<argc; i++) {
    printf(" %s",argv[i]);
  }
  printf("\n");
  return 0;
}
