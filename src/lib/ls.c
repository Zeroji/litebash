#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <strings.h>

/*
 This program display the same thing as the original unix command "ls",
 it means he display the name of each files in the directory given.
 He only allows to see or not the hidden files (-a) on the directory given as an arguments (as the original ls)

 This program isn't finish yet.
 */

LIST("ls")

const int TAILLE_MAX = 100;
const int TAILLE_MAX_ELEM = 250; // nombre de caractère maximal d'un nom de dossier/fichier



void display_ls(char **l_fichier, int taille) {
  int m = 0;
  while (m != taille) {
    printf("%s\n", l_fichier[m]);
    m++;
  }
}
int compareFile(void const *a_file, void const *b_file) {

  //Déclaration des pointeurs sur les noms de fichier
  char const *const *pa_file = a_file;
  char const *const *pb_file = b_file;
  return strcasecmp(*pa_file, *pb_file);
  }

DEF(ls) { //argv[n] contient le chemin d'accès à ls
  //Initialisation du tableau qui contiendra les noms de fichier à afficher
  char *l_fichier[TAILLE_MAX];
  int i = 0; //Initialisation de l'itérateur du tableau

    DIR *dirp;
    struct dirent *dptr;
    int n = 1; // variable qui contiendra le rang du chemin à ls
    int fichier_cache = 0;
    char f_cache[] = "-a";

    // On cherche l'emplacement du pathname
    while ((n < argc) && ((argv[n][0] == '-') && (argv[n][0] != '/'))) {
      // Si on veut montrer les fichiers caché
      if (strcasecmp(argv[n],f_cache) == 0) {
        fichier_cache = 1;
      }
      n++;
    }


    if (n < argc) { //Si on répertoire est donnée
      dirp = opendir(argv[n]);
    }
    else { // Sinon on affiche les fichiers du répertoire courant
      dirp = opendir(".");
    }

    // On lit chaque fichier du répertoire
    while ((dptr=readdir(dirp))) {
      // Si on veut montrer les fichiers cachés, on affiche tout
      if (fichier_cache == 1) {
        l_fichier[i] = dptr->d_name;
        i++;
      }
      else {
        //Sinon dès que le premier caractère du nom de fichier est un point, on affiche pas le nom de fichier
        if (dptr->d_name[0] != '.') {
          l_fichier[i] = dptr->d_name;
          i++;
        }
      }
    }


    //Affichage avant le tri
    display_ls(l_fichier, i);
    printf("\n");
    //Tri du tableau de fichier
    qsort(l_fichier, i, sizeof(*l_fichier), compareFile);
    //Affichage du tableau de fichier
    display_ls(l_fichier, i);

    //Libération de mémoire
    closedir(dirp);


    return 0;
}
