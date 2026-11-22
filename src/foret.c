/* foret.c : partie 2, étapes 4 à 6 — la forêt, ses grilles d'occupation et de lumière, sa pousse (code fourni, à compléter) */

#include <math.h>
#include "foret.h"

/* ---------------------------------------------------------------- code fourni */

int columnOf(t_foret *f, float x)
{
    // en C, -1 % 12 vaut -1 : on ramène le reste entre 0 et C - 1
    int c = (int) floorf(x) % f->C;

    if (c < 0)
    {
        c += f->C;
    }

    return c;
}

int rowOf(t_foret *f, float y)
{
    int r = (int) floorf(y);

    if (r >= f->HMAX)
    {
        r = f->HMAX - 1;
    }
    else if (r < 0)
    {
        r = 0;      // ne se produit pas (un fils est toujours plus haut que son parent) : simple précaution
    }

    return r;
}

/* ---------------------------------------------------------------- à compléter : vos fonctions, à partir d'ici */

#include <stdlib.h>
#include <stdio.h>
#include "croissance_lumiere.h"
#include "alea.h"

static void *checkedCalloc(size_t n, size_t size)
{
    void *p = calloc(n, size);
    if (!p) { perror("Allocation foret"); exit(EXIT_FAILURE); }
    return p;
}

t_foret createForest(int K, int E, int HMAX, double A, int P_max)
{
    t_foret f;
    f.K = K;
    f.E = E;
    f.C = K * E;
    f.HMAX = HMAX;
    f.A = A;
    f.P_max = P_max;
    f.saison = 0;
    f.arbres = checkedCalloc((size_t)K, sizeof(*f.arbres));
    f.occupation = checkedCalloc((size_t)HMAX, sizeof(*f.occupation));
    f.lumiere = checkedCalloc((size_t)HMAX, sizeof(*f.lumiere));
    for (int r = 0; r < HMAX; ++r) {
        f.occupation[r] = checkedCalloc((size_t)f.C, sizeof(**f.occupation));
        f.lumiere[r] = checkedCalloc((size_t)f.C, sizeof(**f.lumiere));
    }
    return f;
}

void addToGrid(t_foret *f, t_rameau *r)
{
    ++f->occupation[rowOf(f, r->y)][columnOf(f, r->x)];
}

void addTreeToGrid(t_foret *f, t_rameau *r)
{
    if (!r) return;
    addToGrid(f, r);
    for (int i = 0; i < r->nb_fils; ++i) addTreeToGrid(f, r->p_fils[i]);
}

void computeLight(t_foret *f)
{
    for (int c = 0; c < f->C; ++c) {
        double L = 1.0;
        for (int r = f->HMAX - 1; r >= 0; --r) {
            f->lumiere[r][c] = L;
            for (int j = 0; j < f->occupation[r][c]; ++j) L *= 1.0 - f->A;
        }
    }
}

double getLight(t_foret *f, t_rameau *r)
{
    return f->lumiere[rowOf(f, r->y)][columnOf(f, r->x)];
}

void plantForest(t_foret *f, int seed, int identique)
{
    for (int i = 0; i < f->K; ++i) {
        float x = (float)(i * f->E + f->E / 2.0);
        f->arbres[i] = createForestTree(i, x, 1.0f, seed);
    }
    randomInit((uint64_t) seed);
    for (int i = 0; i < f->K; ++i) {
        t_arbre_foret *a = &f->arbres[i];
        if (!identique) {
            float h = (float)randomBetween(0.5, 1.5);
            a->step.arbre.pied->y = h;
            a->step.arbre.pied->longueur = h;
            a->cime = h;
        }
        addToGrid(f, a->step.arbre.pied);
    }
}

void growSeasonInForest(t_foret *f, t_arbre_foret *a)
{
    ++a->step.saison;
    if (a->tronque) return;
    t_queue ancienne = a->step.bourgeons;
    t_queue suivante = createEmptyQueue();
    while (!isEmptyQueue(ancienne)) {
        t_rameau *b = dequeue(&ancienne);
        double L = getLight(f, b);
        if (canGrowInLight(L)) {
            int n = howManyBranchesInLight(L);
            growBudInLight(a, b, n, L);
            for (int i = 0; i < b->nb_fils; ++i) {
                t_rameau *fils = b->p_fils[i];
                addToGrid(f, fils);
                enqueue(&suivante, fils);
            }
        } else {
            enqueue(&suivante, b);
        }
    }
    a->step.bourgeons = suivante;
    if (a->step.arbre.nb_rameaux >= f->P_max) a->tronque = 1;
}

void growForestSeason(t_foret *f)
{
    computeLight(f);
    ++f->saison;
    for (int i = 0; i < f->K; ++i) growSeasonInForest(f, &f->arbres[i]);
}

void simulateForest(t_foret *f, int S)
{
    for (int s = 0; s < S; ++s) growForestSeason(f);
}

void freeForest(t_foret *f)
{
    for (int i = 0; i < f->K; ++i) {
        if (f->arbres[i].step.arbre.pied) freeForestTree(&f->arbres[i]);
    }
    free(f->arbres);
    for (int r = 0; r < f->HMAX; ++r) {
        free(f->occupation[r]);
        free(f->lumiere[r]);
    }
    free(f->occupation);
    free(f->lumiere);
    f->arbres = NULL;
    f->occupation = NULL;
    f->lumiere = NULL;
}
