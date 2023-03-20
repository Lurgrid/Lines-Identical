//  Partie interface du module opt (gestion d'option).
//
//  Ce module permet de mettre en place au sein d'un programme, une gestion
//    d'options pour son utilisateur.

#ifndef OPT__H
#define OPT__H

#include <stdbool.h>
#include <stdlib.h>

//  struct optparam optparam : type et nom de type d'un contrôleur regroupant
//    les informations nécessaire pour gérer une option.
typedef struct optparam optparam;

//  optreturn : enumaration des valeurs de retours possible de la fonctions
//    opt_init. (chaque utilisation des valeurs des enumes est spécifier dans la
//    spécification de la fonction opt_init)
typedef enum {
  NO_PARAM,
  STOP_PROCESS,
  ERROR_FUN,
  ERROR_PARAM,
  DONE,
} optreturn;

//  opt_gen : Tente d'allouer les resources néssaires pour gérer une options.
//    Avec pour valeur d'attribut minimum optshort, optlong qui représente
//    respectivement les version courte et longue de l'option. desc, une
//    chaine de carractère, la description de l'option. Un booléen qui vaut vrai
//    si l'option demande un paramètre et faux sinon. la fonction fun représente
//    les actions l'ors de l'appel de l'option, avec cntxt un pointeur vers un
//    context, value le possible paramettre de l'option à traiter et err un
//    pointeur de chaine de caractère qui change de valeur en cas d'erreur.
//    Renvoie NULL en cas de dépassement de capacité. Renvoie sinon un pointer
//    vers la structure associé a cette option.
extern optparam *opt_gen(const char *optshort, const char *optlong,
    const char *desc, bool arg, int (*fun)(void *cntxt, const char *value, const char **err));

//  opt_dispose : Sans effet si *optptr vaut NULL, sinon libère les ressources
//    allouées à la gestion du paramèttre pointé par optptr et met *optptr à
//    NULL.
extern void opt_dispose(optparam **optptr);

//  opt_init : Pour toutes les options représenté dans le tableau argv de
//    longueur argc (valeur argv[0] exclu, car devant représenté le nom de
//    l'executable), qui sont présente dans le tableau d'option aopt, effectue
//    le traitement adequatement definie a la création de l'option (avec comme
//    paramettre a ce traitement cntxt, err un pointeur sur une chaine constante
//    qui sera mis à jour en cas d'erreur lors du traitement, et si l'option
//    necessite un autre argument alors passe la étant censé représenter le
//    paramettre de l'option en argument, sinon la chaine représentant
//    l'option). Si ce traitement echoue par manque d'un paramettre à l'option
//    actuellement executer, alors la fonction retourne ERROR_PARAM et *err pointe sur le nom de l'option dont il manque un paramettre. Si le
//    traitement de l'option echoue alors, la fonction retourne ERROR_FUN. Dans
//    tous les cas d'erreur, le pointeur *err, est mis à jour, avec une chaine
//    représentant l'erreur intervenue. Sinon en cas de succés du traitement de
//    toutes les options, retourne DONE et *err est mis à la valeur NULL. Une
//    options par default existe, celle du "help" (-h et --help pour y accéder.
//    Cette option affiche la façon dont il faut utiliser le programme argv[0],
//    usage, la liste des options du tableau aopt et la description du programme
//    desc) si cette options est appeler, la fonction retourne STOP_PROCESS,
//    *err est mis à la valeur NULL,  de plus la fonction n'a donc pas effectuer
//    les traitement des options pas encore traiter. Pour toutes chaine de
//    caractère présente dans argv qui ne correspont ni à une options ni a
//    l'arguments d'une options, le traitement de ces chaines est assurer par la
//    fonction other qui prend en paramettre cette chaine (dans l'argument
//    value), cntxt (un pointeur sur un context) et err (de même spécification
//    que précédament) et qui retourne une valeur nul en cas de succés, non nul
//    en cas d'echec ce qui amène au retour ERROR_FUN pour la fonction opt_init.
extern optreturn opt_init(char **argv, int argc, optparam **aopt,
    size_t nmemb, int (*other)(void *cntxt, const char *value,
    const char **err), void *cntxt, const char **err, const char *usage,
    const char *desc);

#endif
