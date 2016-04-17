/*
 *
 *  Created on: 6 avr. 2016
 *      Author: corentin
 */

#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <grp.h>

LIST("chown");

const int CHAR_MAX = 1024;

DEF(chown) {
	extern int errno; // Variable afin de faire apparaître les erreurs errno lors de l'utilisation de chown
	int dernier = argc-1;
	// S'il n'y a pas d'argument on renvoie une erreur
	if (argc > 2) { //Pour fonctionner chown doit avoir au minimum 2 arguments
		// Etablissement du pathname
		struct stat file;

		//Création du pathname
		char pathname[CHAR_MAX];
		getcwd(pathname,sizeof(pathname));
		if (argv[dernier][0]!='/' && argv[dernier][0] != '.') { // Si le premier caractère de la racine est différents de / et . , c'est donc seulement le nom de fichier dans le dossier courant
			strcat(pathname,"/");
			strcat(pathname,argv[dernier]);
		} else if (argv[dernier][0]=='.' && argv[dernier][1]=='/') { // Si les premiers caractères sont ./  , c'est donc un chemin relatif
			int j;
			int n=(int)strlen(argv[dernier]);
			strcat(pathname,"/");
			//Concaténer avec le nom de fichier
			for (j=2;j<n;j++) {
				sprintf(pathname,"%s%c",pathname,argv[dernier][j]);
			}
		} else { //C'est un chemin absolu, il n'y donc rien à changer
			sprintf(pathname,"%s",argv[dernier]);
		}

		//Utilisation chown
		stat(pathname, &file);
		if (argc == 3) { //Nous sommes dans le cas où on veut seulement changer le owner
			struct passwd *nam_owner = getpwnam((const char *)argv[argc-2]); //création d'un structure qui donne des informations sur un utilisateur
			if (chown(pathname,nam_owner->pw_uid,file.st_gid)==-1) {  //on change seulement le ower
				printf("%d\n",errno);
			}
		} else if (argc == 4) { //Cas où on change le owner et le group
			stat(pathname, &file);
			struct passwd *nam_owner = getpwnam((const char *)argv[argc-3]);
			struct group *nam_group = getgrnam((const char *)argv[argc-2]);
			if (chown(pathname,nam_owner->pw_uid,nam_group->gr_gid)==-1) { //on change owner et group
				printf("%d\n",errno);
			}
		} else {printf("Trop d'arguments consignés");}
	} else {printf("Un argument est manquant");}
	return -1;

	}
