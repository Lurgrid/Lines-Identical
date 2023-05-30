//  bst.h : partie interface d'un module polymorphe pour la spécification ABINR
//    du TDA ABinR(T).

//  AUCUNE MODIFICATION DE CE SOURCE N'EST AUTORISÉE.

#ifndef BST__H
#define BST__H

#include <stdlib.h>
#include <stdio.h>

//  Fonctionnement général :
//  - la structure de données ne stocke pas d'objets mais des références vers
//      ces objets. Les références sont du type générique « void * » ;
//  - si des opérations d'allocation dynamique sont effectuées, elles le sont
//      pour la gestion propre de la structure de données, et en aucun cas pour
//      réaliser des copies ou des destructions d'objets ;
//  - les fonctions qui possèdent un paramètre de type « bst * » ou « bst ** »
//      ont un comportement indéterminé lorsque ce paramètre ou sa déréférence
//      n'est pas l'adresse d'un contrôleur préalablement renvoyée avec succès
//      par la fonction bst_empty et non révoquée depuis par la fonction
//      bst_dispose ;
//  - aucune fonction ne peut ajouter NULL à la structure de données ;
//  - les fonctions de type de retour « void * » renvoient NULL en cas d'échec.
//      En cas de succès, elles renvoient une référence de valeur actuellement
//      ou auparavant stockée par la structure de données ;
//  - l'implantation des fonctions dont la spécification ne précise pas qu'elles
//      doivent gérer les cas de dépassement de capacité ne doivent avoir
//      affaire avec aucun problème de la sorte.

//  struct bst, bst : type et nom de type d'un contrôleur regroupant les
//    informations nécessaires pour gérer un arbre binaire de recherche de
//    d'objets quelconques.
typedef struct bst bst;

//  bst_empty : tente d'allouer les ressources nécessaires pour gérer un nouvel
//    arbre binaire de recherche initialement vide. La fonction de comparaison
//    des objets via leurs références est pointée par compar. Renvoie NULL en
//    cas de dépassement de capacité. Renvoie sinon un pointeur vers le
//    contrôleur associé à l'arbre.
extern bst *bst_empty(int (*compar)(const void *, const void *));

//  bst_dispose : sans effet si *tptr vaut NULL, Libère sinon les ressources
//    allouées à la gestion de l'arbre binaire de recherche associé à *tptr puis
//    affecte NULL à *tptr.
extern void bst_dispose(bst **tptr);

//  bst_add_endofpath : renvoie NULL si ref vaut NULL. Recherche sinon dans
//    l'arbre binaire de recherche associé à t la référence d'un objet égal à
//    celle de référence ref au sens de la fonction de comparaison. Si la
//    recherche est positive, renvoie la référence trouvée. Tente sinon
//    d'ajouter la référence selon la méthode de l'ajout en bout de chemin ;
//    renvoie NULL en cas de dépassement de capacité ; renvoie sinon ref.
extern void *bst_add_endofpath(bst *t, const void *ref);

extern void *bst_add_r(bst *t, const void *ref);

//  bst_remove_climbup_left : recherche dans l'arbre binaire de recherche
//    associé à t la référence d'un objet égal à celle de référence ref au sens
//    de la fonction de comparaison. Si la recherche est négative, renvoie NULL.
//    Retire sinon la référence trouvée selon la méthode du retrait par remontée
//    gauche et renvoie la référence trouvée.
extern void *bst_remove_climbup_left(bst *t, const void *ref);

//  bst_search : recherche dans l'arbre binaire de recherche associé à t la
//    référence d'un objet égal à celle de référence ref au sens de la fonction
//    de comparaison. Renvoie NULL si la recherche est négative, la référence
//    trouvée sinon.
extern void *bst_search(bst *t, const void *ref);

//  bst_size, bst_height, bst_distance : renvoie la taille, la hauteur, la
//    distance de l'arbre binaire de recherche associé à t.
extern size_t bst_size(bst *t);
extern size_t bst_height(bst *t);
extern size_t bst_distance(bst *t);

//  bst_number, bst_rank : calcule le numéro, le rang du nœud de l'arbre binaire
//    de recherche associé à t dont la valeur est égale à celle de l'objet
//    pointé par ref au sens de la fonction de comparaison. Renvoie ce numéro,
//    ce rang si une telle valeur existe. Renvoie sinon le numéro, le rang
//    qu'aurait le nœud si la référence ref était ajoutée à l'arbre.
extern size_t bst_number(bst *t, const void *ref);
extern size_t bst_rank(bst *t, const void *ref);

//  bst_repr_graphic : affiche sur la sortie standard une représentation
//    graphique de l'arbre binaire de recherche associé à t. La fonction put est
//    utilisée pour l'affichage de chacune des valeurs dont les références sont
//    stockées dans l'arbre. La fonction bst_repr_graphic est libre d'afficher
//    des informations supplémentaires liées  aux ous-arbres.
extern void bst_repr_graphic(bst *t, void (*put)(const void *ref));

//  bst_dft_infix_apply_context : parcourt l'arbre binaire de recherche associé
//    à t en profondeur, de manière infixe, de la gauche vers la droite ou de
//    la droite vers la gauche selon que la valeur de dir est inférieure ou
//    égale à zéro ou non. Pour chaque sous-arbre non vide p, exécute
//    fun_pre(context) avant de parcourir le premier sous-arbre de p, puis
//    fun(context, ref) sur la référence stockée à la racine de p, puis
//    fun_post(context) après avoir parcouru le second sous-arbre de p. Si l'une
//    des fonctions fun_pre, fun ou fun_post renvoie une valeur non nulle,
//    l'exécution prend fin et la fonction bst_dft_infix_left_apply_context
//    renvoie cette valeur. Sinon, la fonction renvoie zéro. Il est permis que
//    fun_pre et fun_post aient la valeur NULL ; dans ce cas, aucune des
//    exécutions correspondantes n'a lieu.
extern int bst_dft_infix_apply_context(bst *t, int dir,
    void *context, int (*fun)(void *context, const void *ref),
    int (*fun_pre)(void *context), int (*fun_post)(void *context));

#endif
