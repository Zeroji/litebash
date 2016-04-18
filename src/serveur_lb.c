#include <sys/types.h>  // pour recv() ;
#include <sys/wait.h>   // pour wait() ;
#include <sys/socket.h> // pour socket() ; recv() ;
#include <netinet/in.h> // struct sockaddr_in ;
#include <stdio.h>      // pour printf() ; tmpfile() ; fprintf() ; fwrite() ; feof() ; ferror() ; fileno() ;
#include <strings.h>    // pour bzero() ;
#include <string.h>     // pour memset() ; strcat() ; strlen() ;
#include <stdlib.h>     // pour exit(); EXIT_SUCCESS et EXIT_FAILURE ;
#include <unistd.h>     // pour write() ; fork() ;
#include <netdb.h>      // pour struct addrinfo ;
#include "parser.h"     // pour parse() ;
#include "shared.h"     // pour BUFFER_SIZE ; BUFFER_SIZE_TO_READ ;
#include <fcntl.h>	// pour fcntl() ;
#include <signal.h>
#include "serveur_lb.h"


/*
variables à externaliser pour pouvoir les fermer dans la fonction fin()
*/
int serveur_socket, client_socket;


LIST("serveur_lb");

DEF(serveur_lb) {
    /*
    === creation des sockets serveur === 
    */
        if ((serveur_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            printf("ERREUR creation socket\n");
            exit(EXIT_FAILURE);
        }
       
        /*
        === attribuer une adresse locale au socket serveur ===
        */
        
        /*
        structure
        */
        struct sockaddr_in serveur_socket_address;

        /*
        mise à zéro des adresses
        */
        bzero(&serveur_socket_address, sizeof(serveur_socket_address));

        /*
        propriété sin_family
        */
        serveur_socket_address.sin_family = AF_INET;

        /*
        propriété sin_addr.s_addr
        */    
        serveur_socket_address.sin_addr.s_addr = INADDR_ANY;

        /*
        Recherche d'un port disponible à partir du port n°1000 
        (oui c'est écrit 999 mais on commence par port++ dans le do-while !)
        */
        int port = 999;
        do {
            port ++;
            printf("port testé : %i\n", port);
            serveur_socket_address.sin_port = htons(port);
        } while(bind(serveur_socket, 
                     (struct sockaddr *)&serveur_socket_address, 
                     sizeof(serveur_socket_address)) != 0);

        printf("port utilisé : %i\n", port);

        /*
        === mise en ecoute de la socket dédiée ====
        */
        const int LEN_QUEUE = 0;
        listen(serveur_socket, LEN_QUEUE);
    
    /*
    === l'appui simultané sur les touches Ctrl et C provoque la 
    fermeture prématurée des sockets ===
    */
        struct sigaction act, o_act ;
        sigaction(SIGINT, NULL, &o_act);
        act.sa_handler = fin;
        act.sa_mask = o_act.sa_mask;
        act.sa_flags = SA_RESTART;
        sigaction(SIGINT, &act, NULL);
        
    /*
    === variables nécessaires pour chaque nouveau processus fils ===
    */
        char buffer_lecture_client[BUFFER_SIZE];
        memset(buffer_lecture_client, '\0', sizeof(buffer_lecture_client));

        char str_lecture_execution_commande_client[2];
        memset(str_lecture_execution_commande_client, '\0', \
               sizeof(str_lecture_execution_commande_client));

        char buffer_serveur_2_client[BUFFER_SIZE];
        memset(buffer_serveur_2_client, '\0', sizeof(buffer_serveur_2_client));
    

        char c_test[2];
        memset(c_test, '\0', sizeof(c_test));

        char c[2];
        memset(c, '\0', sizeof(c));

        char path[BUFFER_SIZE];
        memset(path, '\0', sizeof(path));
        
        char prompt[BUFFER_SIZE];
        memset(prompt, '\0', sizeof(prompt));
        
        int i;
        int sig_client;

        enum Etats {
            EAffichage,
            ELecture_cmd,
            ELecture_input,
            EExecution,
            EEcriture
        };

        FILE *pFile_in;                    
        FILE *pFile_out;
        FILE *pFile_err;

        int sig_wait;
           
    while(1) {
        /*
        === connexion avec le socket d'1 client ===
        */
            struct sockaddr_in client_socket_address;

            socklen_t serveur_socket_address_longueur = sizeof(client_socket_address);
            client_socket = accept(serveur_socket,
                                (struct sockaddr *)&client_socket_address,
                                &serveur_socket_address_longueur);
        
        if (fork() == 0) {
            /* 1 FILS POUR 1 CLIENT */

            enum Etats curEtat = EAffichage;
            
           while(1){//while (recv(client_socket, c_test, 1, MSG_PEEK) > 0) { 
                /* tant que le client n'a pas fermé sa socket */

                switch(curEtat) {
                case EAffichage :
                    sig_wait = waitpid(-1, NULL, WNOHANG);
                    switch (sig_wait) {
                        case 0 :
                            /* enfant(s) toujours vivant(s) */
                            /* on place les descripteurs de fichiers au début pour faire comme si c’était de nouveaux fichiers, mais sans modifier les descripteurs de fichiers qui doivent être conservés pour une bonne utilisation de parse() */
                            fseek(pFile_in, 0, SEEK_SET);
                            fseek(pFile_out, 0, SEEK_SET);
                            //fseek(pFile_err, 0, SEEK_SET);
                            curEtat = ELecture_input;
                        break;
                    
                        case -1 :
                            /*
                            === Affichage cmd ===
                            */
                                getcwd(path, BUFFER_SIZE_TO_READ);
                                sprintf(prompt, "lb %s $ ", path);
                                write(client_socket, prompt, sizeof(prompt));
                                /* Création des fichiers stdin / stdout / stderr associés à cette cmd */
                                pFile_in = tmpfile();
                                int fd = fileno(pFile_in);
                                int flags;
                                flags = fcntl(fd, F_GETFL, 0);
                                flags &= ~O_NONBLOCK; 
                                /* retire le flag O_NONBLOCK s'il est présent, ne fait rien sinon */
                                fcntl(fd, F_SETFL, flags);
                                
                                pFile_out = tmpfile();
                                pFile_err = tmpfile();
                                curEtat = ELecture_cmd;
                        break;
                        
                        default :
                                /* case > 0 : */
                                /* fils terminé(s) */
                                fclose(pFile_in);
                                fclose(pFile_out);
                                fclose(pFile_err);
                        break;
                    }

                break;

                case ELecture_cmd :
                    /*
                    === LECTURE ===
                    */
                        i = 0;
                        do {
                            sig_client = recv(client_socket, c, 1, MSG_WAITALL);
                            if (sig_client == -1) {
                                printf("ERROR : An error occured while reading.\n");
                                printf("Fermeture des sockets\n");
                                close(client_socket);
                                close(serveur_socket);
                                exit(EXIT_FAILURE);
                            }
                            if (sig_client == 0) {
                                printf("ERROR : the client closed his socket…\n");
                                printf("Fermeture des sockets\n");
                                close(client_socket);
                                close(serveur_socket);
                                exit(EXIT_FAILURE);
                            }
                            
                            if (i == 0) {
                                strcpy(buffer_lecture_client, c);
                            }
                            else {
                                strcat(buffer_lecture_client, c);
                            }
                            
                            i++;
                        } while (i < BUFFER_SIZE_TO_READ \
                            && c[0] != '\0' \
                            && c[0] != '\n');
               
                        curEtat = EExecution;
                break;

                case ELecture_input :
                    /*
                    === LECTURE ===
                    */
                        i = 0;
                        do {
                            sig_client = read(client_socket, c, 1);
                            if (sig_client == -1) {
                                printf("ERROR : An error occured while reading.\n");
                                printf("Fermeture des sockets\n");
                                close(client_socket);
                                close(serveur_socket);
                                exit(EXIT_FAILURE);
                            }
                            if (sig_client == 0) {
                                printf("ERROR : the client closed his socket…\n");
                                printf("Fermeture des sockets\n");
                                close(serveur_socket);
                                close(client_socket);
                                exit(EXIT_FAILURE);
                            }
                            
                            if (i == 0) {
                                strcpy(buffer_lecture_client, c);
                            }
                            else {
                                strcat(buffer_lecture_client, c);
                            }
                            
                            i++;
                        } while (i < BUFFER_SIZE_TO_READ \
                            && c[0] != '\0' \
                            && c[0] != '\n');
                    
                        fwrite(buffer_lecture_client, 1, sizeof(buffer_lecture_client), pFile_in);
                        curEtat = EEcriture;
                break;


                case EExecution :                
                    /*
                    === EXECUTION ===
                    */                   
                        if (fork() == 0) {
                            /* un fils va se charger de l'exécution de la commande
                            pendant que son père va afficher le contenu de stdout 
                            sur la stdin du client */
                            
                            parse(buffer_lecture_client, pFile_in, pFile_out, pFile_err);                                       
                            fclose(pFile_in);
                            exit(EXIT_SUCCESS);
                        }
     
                        curEtat = EEcriture;
                break;

                case EEcriture :
                    /*
                    === RETOUR ===
                    */
                        i = 0;
                        do {
                            str_lecture_execution_commande_client[0] = fgetc(pFile_out);
                            if (i == 0) {
                                strcpy(buffer_serveur_2_client, str_lecture_execution_commande_client);
                            }
                            else {
                                strcat(buffer_serveur_2_client, str_lecture_execution_commande_client);
                            }
                           
                            if (ferror(pFile_out) != 0) {
                                printf("An error occurred on the server\n");
                                printf("Fermeture des sockets\n");
                                close(serveur_socket);
                                close(client_socket);
                                exit(EXIT_FAILURE);
                            }
                            i++;
                        } while (feof(pFile_out) == 0 \
                                 && i < BUFFER_SIZE_TO_READ);
                        
                        
                        write(client_socket, buffer_serveur_2_client, \
                              sizeof(buffer_serveur_2_client));
                        curEtat = EAffichage;
                break;

                default:
                break;
                }
            }
            /* le client est parti on peut terminer le processus */
            exit(EXIT_SUCCESS);
        }
        else {
            wait(NULL); /* le père attend chacun de ses fils */
            close(client_socket); /* fonctionnement d'un socket (~ pipe) */
        }
    }
    /* fin du serveur */
    printf("Fermeture des sockets\n");
    close(serveur_socket);
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void fin(int sig) {
    printf("Got SIGINT\n");
    printf("Fermeture des sockets\n");
    close(serveur_socket);
    close(client_socket);
    exit(EXIT_FAILURE);
}
