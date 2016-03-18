# Mini-shell : Interface

Interpréteur : Guillaume Dousset & Félix Galliot  
Commandes : Corentin Marie & Hippolyte Pruvost

### Différents modes de fonctionnement

Le Makefile gère le type de compilation, grâce à différents noms de fichiers pour les différents modes : par exemple, `make bash_static`.
- Classique : un exécutable pour le shell, d'autres pour les commandes
- Statique : un exécutable *standalone* pour le shell, avec les commandes intégrées
- Dynamique : un exécutable pour le shell, et des librairies dynamiques (\*.so) pour les commandes

### Fonctionnement général

L'interpréteur extrait de l'entrée standard le nom de la commande, et l'envoie à l'interface (présentement, une fonction) qui retourne un pointeur sur la fonction exécutant la commande.  
Si la commande n'est pas trouvée localement, l'interpréteur cherche dans les différents répertoires listés dans la variable d'environnement `PATH` pour exécuter les commandes standard.
Un message d'erreur est affiché si la commande n'est pas trouvée.

L'interpréteur gère les redirections et flux d'entrée/sortie, les commandes sont simplement définies comme des fonctions `(int)(int, char**)` prenant en argument un nombre de paramètres et un tableau de chaînes de caractères pour les paramètres (comme `main(argc, argv)`).

### Interface classique

L'interface vérifie que l'interpréteur a accès en exécution au programme demandé, grâce à la commande `access(name, X_OK)`. Si le fichier existe, elle renvoie un pointeur vers une fonction interne `int _exec(int argc, char *argv[])` qui exécute `argv[0]`. *Ce comportement est hautement susceptible d'être changé plus tard.*

### Interface statique

Une structure (actuellement un tableau) contient les noms de toutes les fonctions, associés à des pointeurs sur lesdites fonctions. Cette structure est initalisée "en dur" dans le code source, et doit être modifiée si l'on veut ajouter une commande à l'exécutable *standalone*.  
Dans le cas présent, l'interface parcourt un tableau de noms jusqu'à trouver le nom demandé à l'indice `i` puis retourne le pointeur de fonction contenu à l'indice `i` d'un second tableau.

### Interface dynamique

On utilise la même structure que pour l'interface statique (il s'agira d'une structure dynamique de taille variable). Au lancement de l'interpréteur, il parcourt un répertoire passé en argument et charge tous les fichers \*.so contenus (si possible). Les librairies contiennent une variable globale `char *_FL[]` contenant la liste des fonctions, par exemple: `char *_FL[] = {"cat", "rev", NULL};`.  
À l'initialisation, l'interpréteur récupère grâce à `dlsym` tous les pointeurs sur les fonctions, et les insère dans la structure. La recherche d'une fonction par son nom fonctionne ensuite comme pour l'interface statique.
