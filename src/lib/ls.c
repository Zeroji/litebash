#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

/*
 This program display the same thing as the original unix command "ls",
 it means he display the name of each files in the directory given.
 He only allows to see or not the hidden files (-a)(-A) on the directory given as an arguments (as the original ls)

 This program isn't finish yet.
 */

const int TAILLE_MAX = 100;

LIST("ls");

void display_ls(char **l_fichier, int taille) {
  int m = 0;
  while (m != taille) {
    printf("%s ", l_fichier[m]);
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
  // Cette variable contiendra les différentes options données par l'utlisateur
  int option;

  //Initialisation du tableau qui contiendra les noms de fichier à afficher
  char *l_fichier[TAILLE_MAX];
  int i = 0; //Initialisation de l'itérateur du tableau

    DIR *dirp;
    struct dirent *dptr;
    int n = 1; // variable qui contiendra le rang du chemin à ls
    int fichier_cache = 0; //Par défaut on ne montre pas les fichiers caché
    int dossier_courantPere = 1; //Par défaut on montre les dossier . et .. si on veut montrer les fichiers cachés

    // On cherche l'emplacement du pathname
    while ((n < argc) && ((argv[n][0] == '-') && (argv[n][0] != '/'))) {
      n++;
    }

    //Partie détermination des options demandé
    do {
      option = getopt(argc, argv, "aA");
      // Si on veut montrer les fichiers caché
      if (option == 'a' || option == 'A') {
        fichier_cache = 1;
      }
      // Si on ne veut pas monter les fichiers .  et ..
      if (option == 'A') {
        dossier_courantPere = 0;
      }
    } while (option != -1);


    // Prise en compte du cas où l'utilisateur ne donne pas d'argument
    if (n < argc) { //Si on répertoire est donnée
      dirp = opendir(argv[n]);
    } else { // Sinon on affiche les fichiers du répertoire courant
      dirp = opendir(".");
    }

    // On lit chaque fichier du répertoire
    while ((dptr=readdir(dirp))) {
      // Si on veut montrer les fichiers cachés
      if (fichier_cache == 1) {
        // et si on ne veut pas montrer les dossiers . et .., alors on affiche tout sauf ces deux dossiers
        if (dossier_courantPere == 0) {
          if ((strcmp(dptr->d_name, ".") != 0) && (strcmp(dptr->d_name, "..") != 0)) {
            l_fichier[i] = dptr->d_name;
            i++;
          }
        } else {
          l_fichier[i] = dptr->d_name;
          i++;
        }

      } else {
        //Sinon dès que le premier caractère du nom de fichier est un point, on affiche pas le nom de fichier
        if ((dptr->d_name[0] != '.') != 0) {
          l_fichier[i] = dptr->d_name;
          i++;
        }
      }
    }


    //Affichage avant le tri
    //display_ls(l_fichier, i);
    //printf("\n");
    //Tri du tableau de fichier
    qsort(l_fichier, (size_t)i, (size_t)sizeof(*l_fichier), compareFile);
    //Affichage du tableau de fichier
    display_ls(l_fichier, i);

    //Libération de mémoire
    closedir(dirp);


    return 0;
}
