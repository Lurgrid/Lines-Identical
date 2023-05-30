//  Partie interface du module optl (gestion des options à la linux).
//
//  Ce module permet de mettre en place, une gestion d'options pour son
//    utilisateur. L'implémentation de la gestion d'option est baser sur celle
//    des commandes linux.

//  LES SEULES MODIFICATION AUTORISÉE DE CE SOURCE CONCERNE LES LIGNES 20 à .

#ifndef OPTL__H
#define OPTL__H

#include <stdbool.h>
#include <stdlib.h>

//  struct optparam optparam : type et nom de type d'un contrôleur regroupant
//    les informations nécessaire à la gestion d'une option.
typedef struct optparam optparam;

//  LONG_JOIN : séparateur entre le mot-clé et l'argument des "longues" options.
#define LONG_JOIN '='

//  NEXT_NOPT : Spécifieur indiquant que l'élément qui le suis ne doit pas être
//    condiérer comme une option.
#define NEXT_NOPT "--"

//  SHORT_HELP, LONG_HELP, DESC_HELP : représentation courte et longue de 
//    l'option help, mais aussi sa description.
#define SHORT_HELP 'h'
#define LONG_HELP "help"
#define DESC_HELP "display this help and exit"

//  opt_init : Tente d'allouer les resources néssaires pour gérer une options.
//    optshort, optlong représente tout deux la possible représentation de
//    l'options sous forme respectivment 'courte' ou 'long'. Une au moins est
//    obligatoire. La non prise en charge d'une options courte se spécifie par
//    le passage du charactère '\0' pour cette arguement de même pour l'option
//    longue avec la valeur NULL. Desc, une chaine de carractère, la description
//    de l'option. Arg le booléen spécifiant le besoin ou non d'un argument pour
//    cette options. Interup un booléen spécifiant si l'option arrête le
//    traitement des autres options après son traitement. hdl, une fonctoin
//    représentent l'actions à éffectuer l'ors de l'appel de l'option, avec
//    cntxt un pointeur vers un context, value le possible paramettre de
//    l'option à traiter et err un pointeur de chaine de caractère qui change de
//    valeur en cas d'erreur.
//    Renvoie NULL en cas de dépassement de capacité ou que optshort et optlong
//    valent tous deux respectivement les valeurs '\0' et NULL. Renvoie sinon un
//    pointer vers la structure associé a cette option.
extern optparam *opt_init(const char optshort, const char *optlong,
    const char *desc, bool arg, bool interup, int (*hdl)(void *cntxt,
    const char *value, const char **err));

//  opt_dispose : Sans effet si *optptr vaut NULL, sinon libère les ressources
//    allouées à la gestion du paramèttre pointé par optptr et met *optptr à
//    NULL.
extern void opt_dispose(optparam **optptr);

//  optreturn : enumaration des valeurs de retours possible de la fonctions
//    opt_init. (chaque valeur est définie dans la spécfication de la fonction
//    opt_process)
typedef enum {
  ERROR_HDL,
  ERROR_AMB,
  ERROR_PARAM,
  ERROR_UNKNOWN,
  ERROR_DEFAULT,
  STOP_PROCESS,
  DONE,
} optreturn;

//  opt_process : argc représente le nombre de chaine présente dans le tableau
//    de chaine de charactère argv. argv est le tableau contenant les éléments à
//    traiter. aopt est un tableau de nmemb option supporter lors de ce
//    traitement. hdl_dlt, traitement pour les chaines de argv n'étant pas des
//    options, pouvant être NULL si l'utilisateur n'a aucun traitement pour les
//    argumenent n'étant pas une option. Cette fonction prend en paramettre un
//    context cntxt, value représente la chaine à traiter et err pointe sur une
//    possible chaine de charactère représentant une erreur si il y a eu une
//    occurence d'une erreur dans la fonction, valant NULL sinon. Une option 
//    courte est préfixé obligatoirement de short_cal puis suivie d'un 
//    charactère alpha numérique, de même, long_cal est préfixe obligatoire des 
//    option longue. Si les option longues ont un paramettre alors LONG_JOIN est 
//    le séparateur entre l'option et son argument. Si NEXT_NOPT apparait, alors
//    la chaine suivante ne sera pas considérer comme une option. Une option 
//    obligatoire est présente, celle de l'aide, représenter SHORT_HELP, 
//    LONG_HELP pour sa représenation longue et courte. Cette option affiche sur
//    la sortie standard, une aide a l'utilisation avec la chaine usage et desc 
//    en sont sein (voir docuement pour plus de détaille sur cette affichage).
//    La fonction effectue le traitement suivant :
//    - si toutes les élément de argv ont été traiter sans erreur
//        .alors renvoie DONE, *err pointe sur NULL
//    - sinon, si lors d'une traitement d'une options:
//        .Par manque du supposé paramettre de l'option alors renvoie 
//          ERROR_PARAM et *err pointe sur la chaine représentant cette option 
//        .L'option n'est pas présente dans aopt alors renvoie ERROR_UNKNOWN et 
//          *err pointe sur la chaine qui n'est donc pas une options
//        .La chaine tentant de représenter une option est ambigue, car elle
//          peut correspondre à plusieur option, alors renvoie ERROR_AMB et *err
//          pointe sur la représenatation qui est ambigue
//        .En cas d'une erreur l'ors du traitement de l'option par sa fonction,
//          alors renvoie ERROR_HDL et *err pointe sur un message représentant 
//            l'erreur.
//        .L'option a bien été traiter et elle spécifie de terminer le traitment
//           alors renvoie STOP_PROCESS
//    - sinon, si lors du traitment d'une chaine qui n'est pas option:
//        .Si une erreur interviant dans la fonction hdl_dlt, alors renvoie
//          ERROR_DEFAULT, et *err pointe sur une chaine représentnat l'option.
extern optreturn opt_process(int argc, char **argv, const optparam **aopt,
  size_t nmemb, int (*hdl_dlt)(void *cntxt, const char *value,
  const char **err), void *cntxt, const char **err, const char *short_cal,
  const char *long_cal, const char *desc, const char *usage);

#endif
