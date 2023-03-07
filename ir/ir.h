//  Partie interface du module ir (infinite array).
//
//  Un tableau infinie permet la mémorisation d'une suite de valeur de même type
//    mais pour un nombre quelquonque de ces objets. La seul limitation dans le
//    nombre d'élément présent dans le tableau, correspond à la limite mémoire
//    disponible par le module.

#ifndef IR__H
#define IR__H

//  Fonctionnement général :
//  - la structure de données stocke des objets de même type. (donc de même
//      taille size).
//  - les fonctions qui possèdent un paramètre de type « ir * » ou
//      « ir ** » ont un comportement indéterminé lorsque ce paramètre ou
//      sa déréférence n'est pas l'adresse d'un contrôleur préalablement
//      renvoyée avec succès par la fonction ir_empty et non révoquée
//      depuis par la fonction ir_dispose ;
//   - chaque appelle a la fonction ir_empty (avec un retours d'un controleur
//      avec succés) doit donner lieu à un appelle en fin d'utilisation du
//      tableau à la fonction ir_dispose, pour une libération propre et
//      controler de la mémoire.

/*problème : l'utilisateur peut mettre des objets de type différent le temps
 * qu'il ont la même taille.*/

#include <stdlib.h>
#include <stdbool.h>

//  struct ir, ir : type et nom de type d'un contrôleur regroupant les
//    informations nécessaires pour gérer un tableau infinie.
typedef struct ir ir;

//  ir_empty : tente d'allouer les ressources nécessaires pour gérer un
//    nouveau tableau initialement vide. Renvoie NULL en cas de dépassement
//    de capacité. Renvoie sinon un pointeur vers le contrôleur associé au
//    tableau.
extern ir *ir_empty(size_t size);

//  ir_dispose : sans effet si *irptr vaut NULL. Libère sinon les
//    ressources allouées la gestion du tableau associé à *irptr, mais aussi les
//    valeur contenu dans celui-ci, si il y en a, puis affecte NULL à *haptr.
extern void ir_dispose(ir **irptr);

//  ir_add : Renvoie NULL si ref est égal à NULL. Sinon essaye de recopier en
//    bout de tableau la valeur pointé par ref, de taille supposer égal au
//    champs size de a.
//    Renvoie ref si l'ajout à bien été éffectuer. Sinon renvoie NULL en cas de
//    dépassement de capacité.
extern void *ir_add(ir *a, const void *ref);

//  ir_cond_left_search : recherche la première occurrence dans le tableau a,
//    d'un composant  qui satisfait une fonction de test. Renvoie l'adresse du
//    composant trouvé en cas de succès, NULL en cas d'échec.
extern void *ir_cond_left_search(ir * a, bool (*cond)(const void *));


#endif
