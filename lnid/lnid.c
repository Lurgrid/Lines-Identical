#include <stdio.h>
#include <stdlib.h>
#include "da.h"
#include "holdall.h"
#include "opt.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "*** Syntax: %s [OPTION]... <file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  int r = EXIT_SUCCESS;
  optreturn ot;
  if ((ot = opt_init(argv, argc, NULL, 0, NULL, NULL, "[OPTION]... <file>",
      "Le projet consiste à écrire un programme en C dont le but est :\n"
      "— si le nom d’un seul fichier figure sur la ligne de commande,"
      " d’afficher, pour chaque ligne de texte non vide apparaissant au moins "
      "deux fois dans le fichier, les numéros des lignes où elle apparait et le"
      "contenu de la ligne ;\n — si au moins deux noms de fichiers figurent sur "
      "la ligne de commande, d’afficher, pour chaque ligne de texte non vide "
      "apparaissant au moins une fois dans tous les fichiers, le nombre "
      "d’occurrences de la ligne dans chacun des fichiers et le contenu de la "
      "ligne.\n L’affichage se fait en colonnes sur la sortie standard. Les "
      "colonnes sont (uniquement) séparées")) != DONE) {
    if (ot == STOP_PROCESS) {
      goto dispose;
    }
    goto error;
  }
error:
  r = EXIT_FAILURE;
dispose:
  return r;
}
