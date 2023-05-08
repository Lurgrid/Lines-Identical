//  Partie interface du module da (dynamique array).
//
//  Un tableau dynamique permet la mémorisation d'une suite de valeur de même
//    type mais pour un nombre quelconque d'éléments. La seul limitation dans le
//    nombre d'élément présent dans le tableau, correspond à la limite mémoire
//    disponible par le module.

#ifndef DA__H
#define DA__H

//  Fonctionnement général :
//  - la structure de données stocke des objets de même type. (donc de même
//      taille size).
//  - les fonctions qui possèdent un paramètre de type « ad * » ou
//      « ad ** » ont un comportement indéterminé lorsque ce paramètre ou
//      sa déréférence n'est pas l'adresse d'un contrôleur préalablement
//      renvoyée avec succès par la fonction da_empty et non révoquée
//      depuis par la fonction da_dispose.
//  - chaque appelle a la fonction da_empty (avec un retours d'un contrôleur
//      avec succès) doit donner lieu à un appelle en fin d'utilisation du
//      tableau à la fonction da_dispose, pour une libération propre et
//      controler de la mémoire.
//  - L'implémentation garantie que les éléments sont contiguue dans la mémoire.

#include <stdlib.h>

//  struct da, da : type et nom de type d'un contrôleur regroupant les
//    informations nécessaires pour gérer un tableau infinie.
typedef struct da da;

//  da_empty : tente d'allouer les ressources nécessaires pour gérer un
//    nouveau tableau initialement vide de futurs éléments de taille size.
//    Renvoie NULL en cas de dépassement de capacité.
//    Renvoie sinon un pointeur vers le contrôleur associé a ce tableau.
extern da *da_empty(size_t size);

//  da_dispose : sans effet si *dptr vaut NULL. Libère sinon les
//    ressources allouées la gestion du tableau associé à *dptr, mais aussi les
//    valeur contenu dans celui-ci, si il y en a, puis affecte NULL à *daptr.
extern void da_dispose(da **dptr);

//  da_add : Renvoie NULL si ref est égal à NULL. Sinon essaye de recopier en
//    bout de tableau la valeur pointé par ref, de taille supposer égal au
//    champs size de d.
//    Renvoie ref si l'ajout à bien été éffectuer. Sinon renvoie NULL en cas de
//    dépassement de capacité.
extern void *da_add(da *d, const void *ref);

//  da_nth : recherche la nième valeur dans le tableau pointé par d.
//    Retourne la référence trouver à l'indice n dans le tableau sinon renvoie
//    NULL.
extern void *da_nth(da *d, size_t n);

//  da_length : Renvoie le nombre d'élément présent dans le tableau pointé par
//    d.
extern size_t da_length(da *d);

//  da_reset : reinitialise le tableau pointé par d.
extern void da_reset(da *d);

#endif
