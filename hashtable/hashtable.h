//  hashtable.h : partie interface d'un module polymorphe pour la spécification
//    TABLE du TDA Table(T, T') dans le cas d'une table de hachage par chainage
//    séparé.

//  AUCUNE MODIFICATION DE CE SOURCE N'EST AUTORISÉE.

//  Le comportement du module est sensible à la définition préalable de la
//    macroconstante HASHTABLE_STATS.

#ifndef HASHTABLE__H
#define HASHTABLE__H

#include <stdlib.h>

//  Fonctionnement général :
//  - la structure de données ne stocke pas d'objets mais des références vers
//      ces objets. Les références sont du type générique « void * » ;
//  - si des opérations d'allocation dynamique sont effectuées, elles le sont
//      pour la gestion propre de la structure de données, et en aucun cas pour
//      réaliser des copies ou des destructions d'objets ;
//  - les fonctions qui possèdent un paramètre de type « hashtable * » ou
//      « hashtable ** » ont un comportement indéterminé lorsque ce paramètre ou
//      sa déréférence n'est pas l'adresse d'un contrôleur préalablement
//      renvoyée avec succès par la fonction hashtable_empty et non révoquée
//      depuis par la fonction hashtable_dispose ;
//  - aucune fonction ne peut ajouter NULL en tant que référence de valeur à la
//      structure de données ;
//  - les fonctions de type de retour « void * » renvoient NULL en cas d'échec.
//      En cas de succès, elles renvoient une référence de valeur actuellement
//      ou auparavant stockée par la structure de données ;
//  - l'implantation des fonctions dont la spécification ne précise pas qu'elles
//      doivent gérer les cas de dépassement de capacité ne doivent avoir
//      affaire avec aucun problème de la sorte.

//  struct hashtable, hashtable : type et nom de type d'un contrôleur regroupant
//    les informations nécessaires pour gérer une table de références de clés et
//    valeurs quelconques.
typedef struct hashtable hashtable;

//  hashtable_empty :  tente d'allouer les ressources nécessaires pour gérer une
//    nouvelle table de hachage initialement vide. La fonction de comparaison
//    des clés via leurs références est pointée par compar et leur fonction de
//    pré-hachage est pointée par hasfun. Renvoie NULL en cas de dépassement de
//    capacité. Renvoie sinon un pointeur vers le contrôleur associé à la table.
extern hashtable *hashtable_empty(int (*compar)(const void *, const void *),
    size_t (*hashfun)(const void *));

//  hashtable_dispose : sans effet si *htptr vaut NULL. Libère sinon les
//    ressources allouées à la gestion de la table de hachage associée à *htptr
//    puis affecte NULL à *htptr.
extern void hashtable_dispose(hashtable **htptr);

//  hashtable_add : renvoie NULL si valref vaut NULL. Recherche sinon dans la
//    table de hachage associée à ht la référence d'une clé égale à celle de
//    référence keyref au sens de la fonction de comparaison. Si la recherche
//    est positive, remplace la référence de la valeur correspondante par valref
//    et renvoie la référence de la valeur qui était auparavant associée à la
//    clé trouvée. Tente sinon d'ajouter le couple (keyref, valref) à la table ;
//    renvoie NULL en cas de dépassement de capacité ; renvoie sinon valref.
extern void *hashtable_add(hashtable *ht, const void *keyref,
    const void *valref);

//  hashtable_remove : recherche dans la table de hachage associée à ht la
//    référence d'une clé égale à celle de référence keyref au sens de la
//    fonction de comparaison. Si la recherche est négative, renvoie NULL.
//    Retire sinon le couple (fkeyref, fvalref) de la table, où fkeyref est la
//    référence de la clé trouvée et fvalref la référence de la valeur
//    correspondante et renvoie fvalref.
extern void *hashtable_remove(hashtable *ht, const void *keyref);

//  hashtable_search :  recherche dans la table de hachage associée à ht la
//    référence d'une clé égale à celle de référence keyref au sens de la
//    fonction de comparaison. Renvoie NULL si la recherche est négative, la
//    référence de la valeur correspondante sinon.
extern void *hashtable_search(hashtable *ht, const void *keyref);

#if defined HASHTABLE_STATS && HASHTABLE_STATS != 0

#include <stdio.h>

//  struct hashtable_stats : structure regroupant quelques informations qui
//    constituent un bilan de santé d'une table de hachage.
struct hashtable_stats {
  size_t nslots;      //  nombre de compartiments
  size_t nentries;    //  nombre de clés
  double ldfactmax;   //  taux de remplissage maximum toléré
  double ldfactcurr;  //  taux de remplissage courant
  size_t maxlen;      //  maximum des longueurs des listes
  double postheo;     //  nombre moyen théorique de comparaisons dans le cas
                      //    d'une recherche positive
  double poscurr;     //  nombre moyen courant de comparaisons dans le cas d'une
                      //    recherche positive
};

//  hashtable_get_stats : effectue un bilan de santé pour la table de hachage
//    associée à ht et affecte le résultat à *htsptr.
extern void hashtable_get_stats(hashtable *ht, struct hashtable_stats *htsptr);

//  hashtable_fprint_stats : effectue un bilan de santé pour la table de hachage
//  associée à ht  et écrit le résultat dans le flot texte lié au contrôleur
//    pointé par textstream. Renvoie une valeur non nulle si une erreur en
//    écriture survient. Renvoie sinon zéro.
extern int hashtable_fprint_stats(hashtable *ht, FILE *textstream);

#endif

#endif
