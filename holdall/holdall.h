//  Partie interface du module holdall (fourretout).
//
//  Un fourretout permet la mémorisation d'une liste de références d'objets
//    quelconques, l'exécution de fonctions sur les objets repérés par les
//    références dans la liste selon l'ordre dans lequel les références y
//    figurent ainsi que la réorganisation de la liste.

//  LA SEULE MODIFICATION AUTORISÉE DE CE SOURCE CONCERNE LA LIGNE xxx.
//  TOUTE ÉVENTUELLE MODIFICATION DE LA LIGNE 108 DOIT SE CONFORMER AUX
//    SPÉCIFICATIONS EXPRIMÉES AUX LIGNES 112-115.

#ifndef HOLDALL__H
#define HOLDALL__H

//  Fonctionnement général :
//  - la structure de données ne stocke pas d'objets mais des références vers
//      ces objets. Les références sont du type générique « void * » ;
//  - si des opérations d'allocation dynamique sont effectuées, elles le sont
//      pour la gestion propre de la structure de données, et en aucun cas pour
//      réaliser des copies ou des destructions d'objets ;
//  - les fonctions qui possèdent un paramètre de type « holdall * » ou
//      « holdall ** » ont un comportement indéterminé lorsque ce paramètre ou
//      sa déréférence n'est pas l'adresse d'un contrôleur préalablement
//      renvoyée avec succès par la fonction holdall_empty et non révoquée
//      depuis par la fonction holdall_dispose ;
//  - l'insertion d'une référence à un fourretout ne donne lieu à aucun test de
//      la valeur de la référence ;
//  - l'endroit où a lieu l'insertion d'une référence à un fourretout n'est pas
//      spécifié.

#include <stdlib.h>

//- STANDARD -------------------------------------------------------------------

//  struct holdall, holdall : type et nom de type d'un contrôleur regroupant les
//    informations nécessaires pour gérer un fourretout.
typedef struct holdall holdall;

//  holdall_empty : tente d'allouer les ressources nécessaires pour gérer un
//    nouveau fourretout initialement vide. Renvoie NULL en cas de dépassement
//    de capacité. Renvoie sinon un pointeur vers le contrôleur associé au
//    fourretout.
extern holdall *holdall_empty(void);

//  holdall_dispose : sans effet si *haptr vaut NULL. Libère sinon les
//    ressources allouées la gestion du fourretout associé à *haptr puis affecte
//    NULL à *haptr.
extern void holdall_dispose(holdall **haptr);

//  holdall_put : tente d'insérer ref au fourretout associé à ha. Renvoie une
//    valeur non nulle en cas de dépassement de capacité. Renvoie sinon zéro.
extern int holdall_put(holdall *ha, void *ref);

//  holdall_count : renvoie le nombre d'insertions effectuées avec succès dans
//    le fourretout associé à ha depuis sa création.
extern size_t holdall_count(holdall *ha);

//  holdall_apply, holdall_apply_context, holdall_apply_context2 : parcourt le
//    fourretout associé à ha en appelant (respectivement) fun(ref),
//    fun2(ref, fun1(context, ref)), fun2(context2, ref, fun1(context1, ref))
//    pour chacune des références ref dans l'ordre dans lequel elles figurent
//    dans le fourretout. Si, lors du parcours, la valeur de l'appel n'est pas
//    nulle, l'exécution de la fonction prend fin et la fonction renvoie cette
//    valeur. Sinon, la fonction renvoie zéro.
extern int holdall_apply(holdall *ha, int (*fun)(void *));
extern int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ref),
    int (*fun2)(void *ref, void *resultfun1));
extern int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ref),
    void *context2, int (*fun2)(void *context2, void *ref, void *resultfun1));

//- EXTENSION ------------------------------------------------------------------

//  Sont ajoutées au standard quelques fonctions qui peuvent être utiles.
//  Certaines de ces fonctions ne peuvent être exprimées selon le standard sans
//    avoir affaire avec un problème du dépassement de capacité que n'est pas
//    que terminal. Il est exigé de leurs implantations qu'elles évitent cet
//    écueil.
//  Les autres pourraient être exprimées selon le standard sans avoir affaire
//    avec un problème de dépassement capacité ou, si elles ont affaire à lui,
//    de manière terminale uniquement. Il est attendu que leurs implantations se
//    montrent plus performantes que toute expression selon le standard.

//  Pour savoir si le module gère l'extension, il suffit à son utilisateurice de
//    tester si la macroconstante HOLDALL_WANT_EXT est définie et si sa macro-
//    évaluation donne un entier qui n'est pas 0.

//  La macroconstante HOLDALL_WANT_EXT ainsi que les fonctions de l'extension
//    doivent se conformer aux spécifications 1) à 4).
//  1) Les fonctions de l'extension ne doivent être ni déclarées ni définies
//    lorsque la macroconstante HOLDALL_WANT_EXT n'est pas définie ou
//    lorsqu'elle est définie et que sa macro-évaluation donne l'entier 0.
//  2) Les fonctions de l'extension doivent être déclarées dans cette partie
//    interface du module et définies dans la partie implantation du module
//    lorsque la macroconstante HOLDALL_WANT_EXT est définie et que sa macro-
//    évaluation donne un entier qui n'est pas 0.
//  3) Si elle doit être définie, la macroconstante HOLDALL_WANT_EXT ne peut
//    l'être que par ce fichier en-tête, uniquement la première fois où celui-ci
//    est inclus et à la ligne 108.
//  4) Les fonctions de l'extension sont celles dont les spécifications et
//    prototypes figurent aux lignes 112-115.

#if defined HOLDALL_WANT_EXT
#error "Only <holdall.h> is allowed to define HOLDALL_WANT_EXT."
#endif

#define HOLDALL_WANT_EXT 1

#if defined HOLDALL_WANT_EXT && HOLDALL_WANT_EXT != 0

//  holdall_sort : trie le fourretout associé à ha selon la fonction compar
//    appliquée aux références qui y ont insérées avec succès.
extern void holdall_sort(holdall *ha,
    int (*compar)(const void *, const void *));

#endif

//------------------------------------------------------------------------------

#endif
