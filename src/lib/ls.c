#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

/*
 This program display the same thing as the original unix command "ls",
 it means he display the name of each files in the directory given.
 He allows to see or not the hidden files (-a)(-A), information on files (-l)(-h) on the directory given as an arguments (as the original ls)

 This program isn't finish yet.
 */
LIST("ls")

const int TAILLE_MAX = 200;

void display_mode(unsigned long mode) {
  char readable_mode[10]; //Chaine de caractère qui recevra le mode
  sprintf(readable_mode,"%lo",mode);

  char droits[4] = "";
  int i; //itérateur pour pouvoir choisir les 3 derniers chiffre du mode
  for (i = (strlen(readable_mode)-3); i<strlen(readable_mode); i++) {
    char tmp[2];
    sprintf(tmp,"%c",readable_mode[i]);
    strcat(droits,tmp);
  }
  // Ecriture des droits
  for (i = 0; i<3 ; i++) {
    if (droits[i] == '0') {printf("---");}
    else if (droits[i] == '1') {printf("--x");}
    else if (droits[i] == '2') {printf("-w-");}
    else if (droits[i] == '3') {printf("-wx");}
    else if (droits[i] == '4') {printf("r--");}
    else if (droits[i] == '5') {printf("r-x");}
    else if (droits[i] == '6') {printf("rw-");}
    else {printf("rwx");}
  }
}

void display_ls(char **l_fichier, int taille, int afficher_info, int readable_format, char *pathname) {
  int m = 0;
  while (m != taille) {
    if (afficher_info == 1) {
    // affichage en mode ls -l

    // déclaration de la structure
      struct stat file;
      // Création du pathname du fichier m
      char carac[2];
      strcpy(carac, "/");
      //printf("%s", pathname);
      char tmp[TAILLE_MAX];
      strcpy(tmp, pathname);
      strcat(tmp,carac);
      strcat(tmp,l_fichier[m]);

      stat(tmp, &file);
      //Affichage du type de fichier

       switch (file.st_mode & S_IFMT) {
       case S_IFBLK:  printf("\nb");            break; //block device
       case S_IFCHR:  printf("\nc");            break; //character device
       case S_IFDIR:  printf("\nd");            break; //directory
       case S_IFIFO:  printf("\np");            break; //FIFO/pipe
       case S_IFLNK:  printf("\nl");            break; //symlink
       case S_IFREG:  printf("\n-");            break; //regular file
       case S_IFSOCK: printf("\ns");            break; //socket
       default:       printf("\n?");            break; //unknown?
       }
      display_mode((unsigned long) file.st_mode); // mode <=> droits sur le fichier
      printf(" %ld ", (long) file.st_nlink); // nombre de liens
      printf("%ld   %ld ",(long) file.st_uid, (long) file.st_gid); // possesseur (UID | GID)
      // Affichage de la taille du fichier
      if (readable_format == 1) { // Si l'option taille compréhensible a été demandé
        long long size = file.st_size;
        long long size_K = size / (long long) 1000;
        long long size_M = size / (long long) 1000000;
        long long size_G = size / (long long) 1000000000;
        if (size < (long long) 1000) {
          printf("%lld ",size);
        } else if (size_K < (long long) 1000) {
          printf("%lldK ",size_K);
        } else if (size_M < (long long) 1000) {
          printf("%lldM ", size_M);
        } else {printf("%lldG ", size_G);}
      } else {
        printf("%lld ",(long long) file.st_size);
      }
			char *tmp_time = ctime(&file.st_mtime);
			tmp_time[strlen(tmp_time)-1]='\0';
      printf("%s %s", tmp_time, l_fichier[m]); // dernière tps de modification et nom de fichier
    } else {
      printf("%s ", l_fichier[m]);

    }
    m++;
  }
  printf("\n");
}

/* fonction utilisateur de comparaison fournie a qsort() */
static int compare (void const *a, void const *b)
{
   /* definir des pointeurs type's et initialise's
      avec les parametres */
   char const *const *pa = (const char**)a;
   char const *const *pb = (const char**)b;

   /* evaluer et retourner l'etat de l'evaluation (tri croissant) */
   return strcasecmp (*pa, *pb);
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
    int afficher_info = 0; //Afficher les informations des fichiers
    int readable_format = 0; //Afficher les tailles de fichier dans des formats plus lisible (1M, 1Go)

    // On cherche l'emplacement du pathname
    while ((n < argc) && ((argv[n][0] == '-') && (argv[n][0] != '/'))) {
      n++;
    }

    char *pathname;
    if (n == argc) {
      pathname = ".";
    } else {
      pathname = argv[n];
    }


    //Partie détermination des options demandés
    do {
      option = getopt(argc, argv, "aAlh");
      // Si on veut montrer les fichiers caché
      if (option == 'a' || option == 'A') {
        fichier_cache = 1;
      }
      // Si on ne veut pas monter les fichiers .  et ..
      if (option == 'A') {
        dossier_courantPere = 0;
      }
      //Si on veut afficher les infos des fichiers
      if (option == 'l' || option == 'h') {
        afficher_info = 1;}
      // Si on veut les tailles des fichiers soient compréhensible
      if (option == 'h') {readable_format = 1;}

    } while (option != -1);


    // Prise en compte du cas où l'utilisateur ne donne pas d'argument
    if (n < argc) { //Si on répertoire est donnée
      dirp = opendir(pathname);
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

    //Tri du tableau de fichier
    qsort(l_fichier, (size_t)i, (size_t)sizeof(*l_fichier), compare);
    //Affichage du ls
    display_ls(l_fichier, i, afficher_info, readable_format, pathname);

    //Libération de mémoire
    closedir(dirp);


    return 0;
}
