#ifndef FICHIERS_FOURNIS_CROISSANCE_STEP_H
#define FICHIERS_FOURNIS_CROISSANCE_STEP_H

#include "arbre.h"
#include "queue.h"

typedef struct s_arbre_step {
    t_arbre arbre;        // l'arbre lui-meme (pied, nombre de rameaux, graine)
    t_queue bourgeons;    // bourgeons qui pourront pousser a la prochaine saison
    int saison;           // nombre de saisons deja ecoulees
} t_arbre_step;

// arbre reduit a son pied (file des bourgeons = le pied), saison 0
t_arbre_step createTreeStep(float x, float y, int seed);

// fait pousser l'arbre pendant une seule saison (en pleine lumiere)
void growTreeStep(t_arbre_step *a);

// vide la file des bourgeons : l'arbre ne pourra plus pousser
void clearBuds(t_arbre_step *a);

#endif
