#include <sys/types.h>  // pour recv() ;
#include <sys/wait.h>   // pour wait() ;
#include <sys/socket.h> // pour socket() ; recv() ;
#include <netinet/in.h> // struct sockaddr_in ;
#include <signal.h>     // pour sigaction() ; struct sigaction ;
#include "serveur_lb.h" // pour fin() ;
#include <stdio.h>      // pour printf() ; tmpfile() ; fprintf() ; 
#include <strings.h>    // pour bzero() ;
#include <string.h>     // pour memset() ; strcat() ; strcmp ; strlen() ;
#include <stdlib.h>     // pour exit(); EXIT_SUCCESS et EXIT_FAILURE ;
#include <unistd.h>     // pour read() ; write() ; fork() ;
#include <netdb.h>      // pour struct addrinfo ;
//#include "parser.h"     // pour parse() ;
#include "shared.h"     // pour BUFFER_SIZE ; BUFFER_SIZE_TO_READ ;

LIST("serveur_lb");

int serveur_socket, client_socket;
/*
il faut les externaliser pour pouvoir les fermer dans la fonction fin()
*/

enum Etats {
    EOn,
    EFin
};

enum Etats curEtat = EOn;

DEF(serveur_lb) {
    /*
    === creation des sockets serveur === 
    */
    if ((serveur_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("echec a la creation : errno #1\n");
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
    Recherche d'un port disponible à partir du port n°1000 (oui c'est écrit 999 
    mais on commence par port++ dans le do-while !)
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
    === protocole ===
    */
    char buffer_lecture_client[BUFFER_SIZE];
    memset(buffer_lecture_client, '\0', sizeof(buffer_lecture_client));

    char str_lecture_execution_commande_client[2];
    memset(str_lecture_execution_commande_client, '\0', sizeof(str_lecture_execution_commande_client));

    char buffer_serveur_2_client[BUFFER_SIZE];
    memset(buffer_serveur_2_client, '\0', sizeof(buffer_serveur_2_client));

    int status; //nécessaire pour wait()

    char c_test[2];
    memset(c_test, '\0', sizeof(c_test));

    char c[2];
    memset(c, '\0', sizeof(c));

    int i;
    int sig_client;

    fpos_t pos;

    while(curEtat != EFin) {
        /*
        === connexion avec le socket d'1 client ===
        */
        struct sockaddr_in client_socket_address;

        socklen_t serveur_socket_address_longueur = sizeof(client_socket_address);
        client_socket = accept(serveur_socket,
                            (struct sockaddr *)&client_socket_address,
                            &serveur_socket_address_longueur);
       
        
        if (client_socket != -1 && fork() == 0) {
            while (recv(client_socket, c_test, 1, MSG_PEEK) != 0) { // tant que le client est en vie
                /*
                chaque fils exécute le protocole pour 1 client
                */                
                
                /*
                === LECTURE ===
                */
                i = 0;
                do {
                    sig_client = recv(client_socket, c, 1, MSG_WAITALL);
                    if (i == 0) {
                        strcpy(buffer_lecture_client, c);
                    }
                    else {
                        strcat(buffer_lecture_client, c);
                    }

                    i++;
                    if (sig_client == -1) {
                        printf("ERROR : An error occured while reading.\n");
                        curEtat = EFin;
                        printf("Socket beeing shutdown\n");
                        shutdown(serveur_socket, SHUT_RDWR);
                        close(serveur_socket);
                        exit(EXIT_FAILURE);
                    }

                    if (sig_client == 0) {
                        printf("ERROR : the client closed his socket…\n");
                        curEtat = EFin;
                        printf("Socket beeing shutdown\n");
                        shutdown(serveur_socket, SHUT_RDWR);
                        close(serveur_socket);
                        exit(EXIT_FAILURE);
                    }
                } while (i < BUFFER_SIZE_TO_READ \
                    && c[0] != '\0' \
                    && c[0] != '\n' \
                    && curEtat != EFin);
                
                printf("Finish reading\n");

                if (curEtat != EFin) {
                    /*
                    === EXECUTION ===
                    */
                    FILE *pFile_in = tmpfile();
                    FILE *pFile_out = tmpfile();     
                    fgetpos(pFile_out, &pos);       
                    parse(buffer_lecture_client, pFile_in, pFile_out, pFile_out);//si bug : utiliser : fprintf(pFile_out, "%s du serveur\n", "hey"); 
                    
                    /*
                    === RETOUR ===
                    */
                    
                    i = 0;
                    fsetpos(pFile_out, &pos);
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
                            exit(EXIT_FAILURE);
                        }
                        i++;
                    } while (feof(pFile_out) == 0 \
                        && curEtat != EFin \
                        && i < BUFFER_SIZE_TO_READ);
                    
                    if (curEtat != EFin) {
                        write(client_socket, buffer_serveur_2_client, i);
                        fclose(pFile_out);
                        fclose(pFile_in);
                    }
                    else {
                        printf("Got SIGINT\n");
                        exit(EXIT_FAILURE);
                        break;
                    }
                }
                else {
                    printf("Got SIGINT\n");
                    exit(EXIT_FAILURE);
                    break;
                }
            }   
            if (curEtat != EFin) {
                exit(EXIT_SUCCESS);
            }
        }
        else {
            wait(&status); /* le père attend chacun de ses fils */
            printf("status = %i\n", status); /* pour s'assurer que chaque fils s'est bien terminé */
        }
    }
    printf("Socket beeing shutdown\n");
    shutdown(serveur_socket, SHUT_RDWR);
    close(serveur_socket);
    exit(EXIT_SUCCESS);
}

void fin(int sig) {
    curEtat = EFin;
}
