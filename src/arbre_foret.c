#include <stdlib.h>

#include "arbre_foret.h"
#include "croissance.h"
#include "croissance_lumiere.h"
#include "alea.h"

float clampDirection(float);    // definie dans croissance.c

int countBranches(t_rameau *r)
{
    int n = 1;

    for (int i = 0; i < r->nb_fils; i++)
    {
        n += countBranches(r->p_fils[i]);
    }

    return n;
}

int computeHeight(t_rameau *r)
{
    int h = r->gen;

    for (int i = 0; i < r->nb_fils; i++)
    {
        int hf = computeHeight(r->p_fils[i]);
        if (hf > h)
        {
            h = hf;
        }
    }

    return h;
}

float computeTop(t_rameau *r)
{
    float t = r->y;

    for (int i = 0; i < r->nb_fils; i++)
    {
        float tf = computeTop(r->p_fils[i]);
        if (tf > t)
        {
            t = tf;
        }
    }

    return t;
}

float computeXMin(t_rameau *r)
{
    float m = r->x;

    for (int i = 0; i < r->nb_fils; i++)
    {
        float mf = computeXMin(r->p_fils[i]);
        if (mf < m)
        {
            m = mf;
        }
    }

    return m;
}

float computeXMax(t_rameau *r)
{
    float m = r->x;

    for (int i = 0; i < r->nb_fils; i++)
    {
        float mf = computeXMax(r->p_fils[i]);
        if (mf > m)
        {
            m = mf;
        }
    }

    return m;
}

int countBuds(t_rameau *r)
{
    int n = 0;

    if (r->nb_fils == 0)
    {
        n = 1;
    }
    else
    {
        for (int i = 0; i < r->nb_fils; i++)
        {
            n += countBuds(r->p_fils[i]);
        }
    }

    return n;
}

void freeBranches(t_rameau *r)
{
    for (int i = 0; i < r->nb_fils; i++)
    {
        freeBranches(r->p_fils[i]);
    }

    free(r->p_fils);    // free(NULL) est sans danger
    free(r);
}

t_arbre_foret createForestTree(int id, float x, float y, int seed)
{
    t_arbre_foret a;

    a.step = createTreeStep(x, y, seed);
    a.id = id;
    a.hauteur = 0;
    a.cime = y;
    a.xmin = x;
    a.xmax = x;
    a.nb_bourgeons = 1;
    a.tronque = 0;

    return a;
}

void freeForestTree(t_arbre_foret *a)
{
    clearBuds(&a->step);
    freeBranches(a->step.arbre.pied);
    a->step.arbre.pied = NULL;
}

void growBudInLight(t_arbre_foret *a, t_rameau *b, int n, double lumiere)
{
    b->p_fils = malloc(n * sizeof(t_rameau *));

    for (int i = 0; i < n; i++)
    {
        float direction;
        float longueur;
        t_rameau *r;

        if (i == 0)
        {
            direction = b->dir + randomBetween(-10, 10);
        }
        else
        {
            int cote = randomInt(2);
            float angle = randomBetween(30, 60);

            direction = (cote == 0) ? b->dir - angle : b->dir + angle;
        }
        longueur = (float) ((0.5 + 0.5 * lumiere) * randomBetween(0.7, 1.3));
        direction = clampDirection(direction);

        r = growBranch(&a->step.arbre, b, a->step.saison, direction, longueur);
        b->p_fils[i] = r;

        // mesures tenues a jour
        if (r->gen > a->hauteur)
        {
            a->hauteur = r->gen;
        }
        if (r->y > a->cime)
        {
            a->cime = r->y;
        }
        if (r->x < a->xmin)
        {
            a->xmin = r->x;
        }
        if (r->x > a->xmax)
        {
            a->xmax = r->x;
        }
    }

    b->nb_fils = (unsigned char) n;
    b->etat = BOIS;
    a->nb_bourgeons += n - 1;       // b n'est plus un bourgeon, ses n fils en sont
}

void growSeason(t_arbre_foret *a, double lumiere)
{
    t_queue F = a->step.bourgeons;
    t_queue G = createEmptyQueue();

    a->step.saison++;

    while (!isEmptyQueue(F))
    {
        t_rameau *b = dequeue(&F);

        if (canGrowInLight(lumiere))
        {
            int n = howManyBranchesInLight(lumiere);
            growBudInLight(a, b, n, lumiere);

            for (int i = 0; i < b->nb_fils; i++)
            {
                enqueue(&G, b->p_fils[i]);
            }
        }
        else
        {
            enqueue(&G, b);
        }
    }

    a->step.bourgeons = G;
}
