#include <stdio.h>          // pour getc() ; stdin ;
#include <errno.h>
#include <signal.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>      // pour uint16_t ;

#include <string.h>         // pour strcpy() ; memset()

#include "connect_lb.h"     // pour fin() ;
#include "shared.h"         // pour BUFFER_SIZE ; BUFFER_SIZE_TO_READ ;



LIST("connect_lb");

int to_server_socket;
enum Etats {
    ELecture,
    EEcriture,
    EFin
};

enum Etats curEtat = EEcriture;

DEF(connect_lb) {
    uint16_t port;
    char SERVEURNAME[16];
    char *strref[16];

    if (argc < 2) {
        printf("pas assez d'args\n");
        exit(EXIT_FAILURE);
    }
    else {
        strcpy(SERVEURNAME, argv[1]);
/*
        "127.0.0.1" = localhost = pour le debug
*/
        printf("serveur name : %s\n", SERVEURNAME);
/*
        sert à vérifier que tous les caractères sont bien transmis
*/
        port = (uint16_t) strtol(argv[2], strref, 10);
        printf("num port : %i\n", port);
/*
        sert à vérifier que l'on récupère bien un int correspondant au 
        numéro du port
*/
    }

    struct sockaddr_in serverSockAddr;
    struct hostent *serverHostEnt;
    long hostAddr;

    bzero(&serverSockAddr, sizeof(serverSockAddr));
    hostAddr = inet_addr(SERVEURNAME);
    if ((long)hostAddr != (long)-1) {
        bcopy(&hostAddr, &serverSockAddr.sin_addr, sizeof(hostAddr));
    }
    else {
        serverHostEnt = gethostbyname(SERVEURNAME);
        if (serverHostEnt == NULL) {
            printf("Echec gethost\n");
            exit(EXIT_FAILURE);
        }
        bcopy(serverHostEnt->h_addr,&serverSockAddr.sin_addr,serverHostEnt->h_length);
    }

    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port = htons(port);  // où port doit être le port du serveur

    /* creation de la socket */
    if ((to_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Echec creation socket client\n");
        exit(EXIT_FAILURE);
    }
    /* requete de connexion */
    if(connect(to_server_socket,
                (struct sockaddr *)&serverSockAddr,
                sizeof(serverSockAddr)) != 0) {
        printf("Echec demande de connection\n");
        exit(EXIT_FAILURE);
    } // il faudrait que le serveur puisse envoyer une erreur..

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

    /* envoie de donne et reception */
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    int i;
    int c;
    char c_str[2];
    memset(c_str, '\0', sizeof(c_str));
    char str[2];
    memset(str, '\0', sizeof(str));
    int sig_serveur;

    while(curEtat != EFin) {
        switch(curEtat) {
            case EEcriture :
                i = 0;
                printf("Enter command : ");
                do {
                    c = getc(stdin);
                    sprintf(c_str, "%c", c);
                    
                    if (i == 0) {
                        strcpy(buffer, c_str);
                    }
                    else {
                        strcat(buffer, c_str);
                    }                    
                    i++;
                }while (c != '\n' \
                        && c != '\0' \
                        && i < BUFFER_SIZE_TO_READ \
                        && curEtat != EFin);
                
                if (curEtat != EFin) {
                    if (i == BUFFER_SIZE_TO_READ) { // >= à cause du dernier i++
                        printf("AVERTISSEMENT : Vous avez atteint la longueur maximale\
                        pour une commande. Uniquement les %i premiers \
                        caracteres saisis seront envoyes au serveur.",
                        BUFFER_SIZE_TO_READ);
                    }

                    printf("buffer being send is : %s\n", buffer);
                    /*
                    debug
                    */
                    write(to_server_socket, buffer, i);

                    curEtat = ELecture;
                }
                break;

            case ELecture :
                i = 0;
                do {
                    sig_serveur = recv(to_server_socket, str, 1, MSG_WAITALL);

                    if (sig_serveur == -1) {
                        printf("ERROR : An error occurred on the server...\n");
                        curEtat = EFin;
                        printf("Socket beeing shutdown\n");
                        shutdown(to_server_socket, SHUT_RDWR);
                        close(to_server_socket);
                        exit(EXIT_FAILURE);
                    }

                    if (i == 0) {
                        strcpy(buffer, str);
                    }
                    else {
                        strcat(buffer, str);
                    }  
                    i++;

                } while (str[0] != '\0' \
                      && str[0] != EOF \
                      && curEtat != EFin);
                
                if (curEtat != EFin) {
                    printf("Server answer : %s\n", buffer);
                    curEtat = EEcriture;
                }
                break;
            
            case EFin :
                break;
                
            default :
                printf("Unknown case. Ending.\n");
                curEtat = EFin;
        }
    }
    printf("Socket being shutdown\n");
    shutdown(to_server_socket, SHUT_RDWR);
    close(to_server_socket);
    exit(EXIT_SUCCESS);
}

void fin(int sig) {
    curEtat = EFin;
}

