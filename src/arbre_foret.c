#include <stdlib.h>
#include <stdio.h>
#include "arbre_foret.h"
#include "croissance_lumiere.h"
#include "alea.h"

/* Fonction de la partie 1, définie dans croissance.c. */
float clampDirection(float dir);

int countBranches(t_rameau *r)
{
    if (!r) return 0;
    int total = 1;
    for (int i = 0; i < r->nb_fils; ++i) total += countBranches(r->p_fils[i]);
    return total;
}

int computeHeight(t_rameau *r)
{
    if (!r) return 0;
    int maximum = r->gen;
    for (int i = 0; i < r->nb_fils; ++i) {
        int h = computeHeight(r->p_fils[i]);
        if (h > maximum) maximum = h;
    }
    return maximum;
}

float computeTop(t_rameau *r)
{
    if (!r) return 0;
    float maximum = r->y;
    for (int i = 0; i < r->nb_fils; ++i) {
        float y = computeTop(r->p_fils[i]);
        if (y > maximum) maximum = y;
    }
    return maximum;
}

float computeXMin(t_rameau *r)
{
    if (!r) return 0;
    float minimum = r->x;
    for (int i = 0; i < r->nb_fils; ++i) {
        float x = computeXMin(r->p_fils[i]);
        if (x < minimum) minimum = x;
    }
    return minimum;
}

float computeXMax(t_rameau *r)
{
    if (!r) return 0;
    float maximum = r->x;
    for (int i = 0; i < r->nb_fils; ++i) {
        float x = computeXMax(r->p_fils[i]);
        if (x > maximum) maximum = x;
    }
    return maximum;
}

int countBuds(t_rameau *r)
{
    if (!r) return 0;
    if (r->nb_fils == 0) return 1;
    int total = 0;
    for (int i = 0; i < r->nb_fils; ++i) total += countBuds(r->p_fils[i]);
    return total;
}

void freeBranches(t_rameau *r)
{
    if (!r) return;
    for (int i = 0; i < r->nb_fils; ++i) freeBranches(r->p_fils[i]);
    free(r->p_fils);
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
    b->p_fils = malloc((size_t)n * sizeof(*b->p_fils));
    if (!b->p_fils) { perror("growBudInLight"); exit(EXIT_FAILURE); }
    for (int i = 0; i < n; ++i) {
        float dir;
        float len;
        if (i == 0) {
            dir = b->dir + randomBetween(-10, 10);
            len = (float)((0.5 + 0.5 * lumiere) * randomBetween(0.7, 1.3));
        } else {
            int cote = randomInt(2);
            float angle = (float)randomBetween(30, 60);
            dir = cote == 0 ? b->dir - angle : b->dir + angle;
            len = (float)((0.5 + 0.5 * lumiere) * randomBetween(0.7, 1.3));
        }
        dir = clampDirection(dir);
        t_rameau *fils = growBranch(&a->step.arbre, b, a->step.saison, dir, len);
        b->p_fils[i] = fils;
        if (fils->gen > a->hauteur) a->hauteur = fils->gen;
        if (fils->y > a->cime) a->cime = fils->y;
        if (fils->x < a->xmin) a->xmin = fils->x;
        if (fils->x > a->xmax) a->xmax = fils->x;
    }
    b->nb_fils = (unsigned char)n;
    b->etat = BOIS;
    a->nb_bourgeons += n - 1;
}

void growSeason(t_arbre_foret *a, double lumiere)
{
    t_queue ancienne = a->step.bourgeons;
    t_queue suivante = createEmptyQueue();
    ++a->step.saison;
    while (!isEmptyQueue(ancienne)) {
        t_rameau *b = dequeue(&ancienne);
        if (canGrowInLight(lumiere)) {
            int n = howManyBranchesInLight(lumiere);
            growBudInLight(a, b, n, lumiere);
            for (int i = 0; i < b->nb_fils; ++i) enqueue(&suivante, b->p_fils[i]);
        } else {
            enqueue(&suivante, b);
        }
    }
    a->step.bourgeons = suivante;
}
