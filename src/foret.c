/* foret.c : partie 2, étapes 4 à 6 — la forêt, ses grilles d'occupation et de lumière, sa pousse (code fourni, à compléter) */

#include <math.h>
#include <stdlib.h>
#include "foret.h"
#include "croissance_lumiere.h"
#include "alea.h"

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
    f.arbres = (t_arbre_foret *) malloc(K * sizeof(t_arbre_foret));

    f.occupation = (int **) malloc(HMAX * sizeof(int *));
    f.lumiere = (double **) malloc(HMAX * sizeof(double *));
    for (int r = 0; r < HMAX; r++)
    {
        f.occupation[r] = (int *) calloc(f.C, sizeof(int));
        f.lumiere[r] = (double *) malloc(f.C * sizeof(double));
        for (int c = 0; c < f.C; c++)
        {
            f.lumiere[r][c] = 1.0;
        }
    }

    return f;
}

void addToGrid(t_foret *f, t_rameau *r)
{
    f->occupation[rowOf(f, r->y)][columnOf(f, r->x)]++;
}

void addTreeToGrid(t_foret *f, t_rameau *r)
{
    addToGrid(f, r);

    for (int i = 0; i < r->nb_fils; i++)
    {
        addTreeToGrid(f, r->p_fils[i]);
    }
}

void computeLight(t_foret *f)
{
    for (int c = 0; c < f->C; c++)
    {
        double l = 1.0;

        // de haut en bas : les bouts d'une case ombragent les rangees du dessous, pas leur propre case
        for (int r = f->HMAX - 1; r >= 0; r--)
        {
            f->lumiere[r][c] = l;
            for (int k = 0; k < f->occupation[r][c]; k++)
            {
                l *= (1.0 - f->A);
            }
        }
    }
}

double getLight(t_foret *f, t_rameau *r)
{
    return f->lumiere[rowOf(f, r->y)][columnOf(f, r->x)];
}

void plantForest(t_foret *f, int seed, int identique)
{
    for (int i = 0; i < f->K; i++)
    {
        f->arbres[i] = createForestTree(i, (float) (i * f->E) + (float) f->E / 2, 1, seed);
    }

    // createTree a reinitialise le generateur a chaque arbre : une seule initialisation, apres
    randomInit((uint64_t) seed);

    if (!identique)
    {
        for (int i = 0; i < f->K; i++)
        {
            float y = (float) randomBetween(0.5, 1.5);

            f->arbres[i].step.arbre.pied->y = y;
            f->arbres[i].step.arbre.pied->longueur = y;
            f->arbres[i].cime = y;
        }
    }

    for (int i = 0; i < f->K; i++)
    {
        addToGrid(f, f->arbres[i].step.arbre.pied);
    }

    f->saison = 0;
}

void growSeasonInForest(t_foret *f, t_arbre_foret *a)
{
    a->step.saison++;

    if (!a->tronque)
    {
        t_queue F = a->step.bourgeons;
        t_queue G = createEmptyQueue();

        while (!isEmptyQueue(F))
        {
            t_rameau *b = dequeue(&F);
            double l = getLight(f, b);

            if (canGrowInLight(l))
            {
                int n = howManyBranchesInLight(l);
                growBudInLight(a, b, n, l);

                for (int i = 0; i < b->nb_fils; i++)
                {
                    enqueue(&G, b->p_fils[i]);
                    addToGrid(f, b->p_fils[i]);
                }
            }
            else
            {
                enqueue(&G, b);
            }
        }

        a->step.bourgeons = G;

        if (a->step.arbre.nb_rameaux >= f->P_max)
        {
            a->tronque = 1;
        }
    }
}

void growForestSeason(t_foret *f)
{
    computeLight(f);

    for (int i = 0; i < f->K; i++)
    {
        growSeasonInForest(f, &f->arbres[i]);
    }

    f->saison++;
}

void simulateForest(t_foret *f, int S)
{
    for (int s = 0; s < S; s++)
    {
        growForestSeason(f);
    }
}

void freeForest(t_foret *f)
{
    for (int i = 0; i < f->K; i++)
    {
        freeForestTree(&f->arbres[i]);
    }
    free(f->arbres);

    for (int r = 0; r < f->HMAX; r++)
    {
        free(f->occupation[r]);
        free(f->lumiere[r]);
    }
    free(f->occupation);
    free(f->lumiere);
}
