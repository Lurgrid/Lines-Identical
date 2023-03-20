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
//      depuis par la fonction da_dispose ;
//   - chaque appelle a la fonction da_empty (avec un retours d'un contrôleur
//      avec succès) doit donner lieu à un appelle en fin d'utilisation du
//      tableau à la fonction da_dispose, pour une libération propre et
//      controler de la mémoire.

#include <stdlib.h>
#include <stdbool.h>

//  struct da, da : type et nom de type d'un contrôleur regroupant les
//    informations nécessaires pour gérer un tableau infinie.
typedef struct da da;

//  da_empty : tente d'allouer les ressources nécessaires pour gérer un
//    nouveau tableau initialement vide de futurs éléments de taille size.
//    Renvoie NULL en cas de dépassement de capacité. Renvoie sinon un pointeur
//    vers le contrôleur associé a ce tableau.
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

//  da_cond_left_search : recherche la première occurrence dans le tableau d,
//    d'un composant  qui satisfait une fonction de test. Renvoie l'adresse du
//    composant trouvé en cas de succès, NULL en cas d'échec.
extern void *da_cond_left_search(da *d, bool (*cond)(const void *));

//  da_nth : recherche la nième valeur dans le tableau pointé par d.
//    Retourne la référence trouver à l'indice n dans le tableau sinon renvoie
//    NULL.
extern void *da_nth(da *d, size_t n);

//  da_length : Renvoie le nombre d'élément présent dans le tableau pointé par
//    d.
extern size_t da_length(da *d);

//  da_reset : vide le tableau pointé par d.
extern void da_reset(da *d);

//  da_cmp : Renvois une valeur nulle si les elements présent dans d et b
//    sont 2 a 2 identique celon la fonction de compaison pointé par compar.
//    Sinon renvoit une valeur non nulle.
extern int da_cmp(da *d, da *b, int (*comp)(const void *, const void *));

//  da_dupli : Renvois un tableau dynamique équivalant a d en cas de réussite.
//    Renvois NULL, en cas de dépassement de capacité.
extern da *da_dupli(da *d);

//  da_display_context : Applique a tout les éléments de da fun avec comme
//    parametre le context cntxt. Si, lors du parcours, la valeur de l'appel
//    n'est pas nulle, l'exécution de la fonction prend fin et la fonction
//    renvoie cette valeur. Sinon, la fonction renvoie zéro.
extern int da_apply_context(da *d, void *cntxt, int (*fun)(void *, void *));

//  da_display : Applique a tout les éléments de da fun. Si, lors du parcours,
//    la valeur de l'appel n'est pas nulle, l'exécution de la fonction prend fin
//    et la fonction renvoie cette valeur. Sinon, la fonction renvoie zéro.
extern int da_apply(da *d, int (*fun)(void *));

#endif
