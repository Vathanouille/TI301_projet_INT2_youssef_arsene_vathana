/* foret.h : partie 2, étapes 4 à 6 — la forêt, ses grilles d'occupation et de lumière, sa pousse (code fourni, à compléter) */

#ifndef FORET_FORET_H
#define FORET_FORET_H

#include "arbre_foret.h"

typedef struct s_foret {
    int K, E, C, HMAX;        // nombre d'arbres, espacement, colonnes (C = K x E), rangees
    double A;                 // part de lumiere retenue par un bout de rameau
    int P_max;                // nombre maximal de rameaux d'un arbre
    t_arbre_foret *arbres;    // tableau dynamique des K arbres
    int **occupation;         // HMAX x C compteurs de bouts de rameaux
    double **lumiere;         // HMAX x C lumieres
    int saison;               // nombre de saisons deja ecoulees
} t_foret;

/* ---------------------------------------------------------------- code fourni (ne compile qu'une fois t_foret écrite) */

/***
 * @brief colonne de la case qui contient l'abscisse x (la forêt fait le tour : modulo C)
 * @param f : la forêt
 * @param x : position, éventuellement négative ou au moins égale à C
 * @return la colonne, entre 0 et C - 1
 */
int columnOf(t_foret *f, float x);

/***
 * @brief rangée de la case qui contient l'altitude y (au moins HMAX : dernière rangée)
 * @param f : la forêt
 * @param y : altitude
 * @return la rangée, entre 0 et HMAX - 1
 */
int rowOf(t_foret *f, float y);

/* ---------------------------------------------------------------- à compléter : vos prototypes, à partir d'ici */

t_foret createForest(int K, int E, int HMAX, double A, int P_max);
void addToGrid(t_foret *f, t_rameau *r);
void addTreeToGrid(t_foret *f, t_rameau *r);
void computeLight(t_foret *f);
double getLight(t_foret *f, t_rameau *r);
void plantForest(t_foret *f, int seed, int identique);
void growSeasonInForest(t_foret *f, t_arbre_foret *a);
void growForestSeason(t_foret *f);
void simulateForest(t_foret *f, int S);
void freeForest(t_foret *f);

#endif //FORET_FORET_H
