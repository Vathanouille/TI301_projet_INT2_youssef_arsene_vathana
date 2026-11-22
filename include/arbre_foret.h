#ifndef FORET_ARBRE_FORET_H
#define FORET_ARBRE_FORET_H

#include "croissance_step.h"

typedef struct s_arbre_foret {
    t_arbre_step step;    // l'arbre, sa file de bourgeons et sa saison
    int id;               // numero de l'arbre dans la foret
    int hauteur;          // plus grande generation de ses rameaux
    float cime;           // plus haute altitude des extremites de ses rameaux
    float xmin, xmax;     // plus petite et plus grande abscisses des extremites
    int nb_bourgeons;     // nombre de bourgeons (nombre d'elements de la file)
    int tronque;          // 1 si l'arbre a atteint P_max rameaux : il ne pousse plus
} t_arbre_foret;

// mesures recursives (sous-arbre de r)
int countBranches(t_rameau *r);
int computeHeight(t_rameau *r);
float computeTop(t_rameau *r);
float computeXMin(t_rameau *r);
float computeXMax(t_rameau *r);
int countBuds(t_rameau *r);
void freeBranches(t_rameau *r);

t_arbre_foret createForestTree(int id, float x, float y, int seed);
void freeForestTree(t_arbre_foret *a);

// cree n rameaux a l'extremite du bourgeon b, a la saison courante, avec la lumiere donnee
void growBudInLight(t_arbre_foret *a, t_rameau *b, int n, double lumiere);

// une saison de pousse, meme lumiere pour tous les bourgeons
void growSeason(t_arbre_foret *a, double lumiere);

#endif
