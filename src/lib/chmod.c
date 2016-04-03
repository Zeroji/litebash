#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

LIST("chmod")

mode_t set_mode(char *arg1, char *arg2, mode_t initial) {
  /*
   * arg1 = droits_avant modification (cas où on a une commande du type "chmod u+x ./x"
   * arg2 = droits_après modification
   */
  //unsigned int L_USR[7]=[S_IXUSR,S_IWUSR,(S_IXUSR|S_IWUSR),S_IRUSR,(S_IXUSR|S_IRUSR),(S_IWUSR|S_IRUSR),(S_IXUSR|S_IWUSR|S_IRUSR)];

  mode_t mode = NULL;
  if (arg2[0] == '0' || arg2[0] == '1' || arg2[0] == '2' || arg2[0] == '3' || arg2[0] == '4' || arg2[0] == '5' || arg2[0] == '6' || arg2[0] == '7' || arg2[0] == '8' || arg2[0] == '9' ) {
    if (strlen(arg2) == 3) {
      // On a donc une commande du type "chmod 055 ./x"

      //User
      if (arg2[0] == '1') {
        mode = mode|S_IXUSR;
      } else if (arg2[0] == '2') {
        mode = mode|S_IWUSR;
      } else if (arg2[0] == '3') {
        mode = mode|S_IXUSR|S_IWUSR;
      } else if (arg2[0] == '4') {
        mode = mode|S_IRUSR;
      } else if (arg2[0] == '5') {
        mode = mode|S_IRUSR|S_IXUSR;
      } else if (arg2[0] == '6') {
        mode = mode|S_IRUSR|S_IWUSR;
      } else if (arg2[0] == '7') {
        mode = mode|S_IRUSR|S_IWUSR|S_IXUSR;
      }
      //Sinon c'est 0 mais dans ce cas là on ne fais rien

      //Group
      if (arg2[1] == '1') {
        mode = mode|S_IXGRP;
      } else if (arg2[1] == '2') {
        mode = mode|S_IWGRP;
      } else if (arg2[1] == '3') {
        mode = mode|S_IXGRP|S_IWGRP;
      } else if (arg2[1] == '4') {
        mode = mode|S_IRGRP;
      } else if (arg2[1] == '5') {
        mode = mode|S_IRGRP|S_IXGRP;
      } else if (arg2[1] == '6') {
        mode = mode|S_IRGRP|S_IWGRP;
      } else if (arg2[1] == '7') {
        mode = mode|S_IRGRP|S_IWGRP|S_IXGRP;
      }
      //Sinon c'est 0 mais dans ce cas là on ne fais rien

      //Other
      if (arg2[2] == '1') {
        mode = mode|S_IXOTH;
      } else if (arg2[2] == '2') {
        mode = mode|S_IWOTH;
      } else if (arg2[2] == '3') {
        mode = mode|S_IXOTH|S_IWOTH;
      } else if (arg2[2] == '4') {
        mode = mode|S_IROTH;
      } else if (arg2[2] == '5') {
        mode = mode|S_IROTH|S_IXOTH;
      } else if (arg2[2] == '6') {
        mode = mode|S_IROTH|S_IWOTH;
      } else if (arg2[2] == '7') {
        mode = mode|S_IROTH|S_IWOTH|S_IXOTH;
      }
      //Sinon c'est 0 mais dans ce cas là on ne fais rien
    } else {
      mode = initial; // Si la commande n'est pas bien écrite (pas le bon nombre de chiffre) on ne change pas les droits du fichier
    }
  }
  return mode;
}


char * display_mode(unsigned long mode, char *droits) {
  char readable_mode[10]; //Chaine de caractère qui recevra le mode
  sprintf(readable_mode,"%lo",mode);
  int i; //itérateur pour pouvoir choisir les 3 derniers chiffre du mode
  for (i = (strlen(readable_mode)-3); i<strlen(readable_mode); i++) {
    char tmp[2];
    sprintf(tmp,"%c",readable_mode[i]);
    strcat(droits,tmp);
  }
  return droits;
}

DEF(chmod) {

  // S'il n'y a pas d'argument on renvoie une erreur
  if (argv[1] != NULL) {
    char droits_avant[4] = "";
    char droits_apres[4] = "";

    // Etablissement du pathname
    char *pathname;
    if (argv[2] == NULL) {
      printf("Fichier manquant");
    } else {
      pathname = argv[2];
      //printf("%s", pathname);
      struct stat file;
      stat(pathname, &file);
      //printf(" %lld",(long long) file.st_size);
      sprintf(droits_avant,"%s",display_mode((unsigned long) file.st_mode, droits_avant));
      //printf(" %s",droits_avant);
      if (chmod(pathname, set_mode(droits_avant,argv[1],file.st_mode)) == -1) {
        printf("Modification error");
      }
      stat(pathname, &file);
      sprintf(droits_apres,"%s",display_mode((unsigned long) file.st_mode, droits_apres));
      //printf(" %s",droits_apres);


    }
  } else {
    printf("Un argument est manquant");
  }
  return 0;
}