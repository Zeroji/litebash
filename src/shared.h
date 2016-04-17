#ifndef SHAREFILE_H
#define SHAREFILE_H
#define BUFFER_SIZE_TO_READ 4096
// ce qui impose que la longueur d'une ligne de commande de l'utilisateur ne dépasse pas non plus cette limite

/*
valeur à corréler avec celle du main
*/
#define BUFFER_SIZE BUFFER_SIZE_TO_READ + 1
/*
+1 pour stocker '\0';
*/
#endif