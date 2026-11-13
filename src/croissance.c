//
// Created by flasque on 12/10/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "croissance.h"
#include "queue.h"

// prototypes des fonctions locales

float clampDirection(float);

// code des fonctions locales

float clampDirection(float dir)
{
    float res = dir;

    if (res < -80)
    {
        res = -80;
    }
    else if (res > 80)
    {
        res = 80;
    }

    return res;
}

// code des fonctions exportées

int canGrow(float probabilite)
{
    return (randomReal() < probabilite / 100.0);
}

int howManyBranches(float p1, float p2)
{
    float u = randomReal();

    if (u < p1 /100.0)
    {
        return 1;
    }
    if (u < (p1+p2) /100.0)
    {
        return 2;
    }
    return 3;

}

void growBud(t_arbre *a, t_rameau *b, int n, int saison)
{
    b->p_fils = malloc(n * sizeof(t_rameau *));

    for (int i = 0; i < n; i++)
    {
        float direction;
        float longueur;

        if (i == 0)
        {
            direction = b->dir + randomBetween(-10, 10);
            longueur = randomBetween(0.7, 1.3);
        }
        else
        {
            int cote = randomInt(2);
            float angle = randomBetween(30, 60);

            if (cote == 0)
                direction = b->dir - angle;
            else
                direction = b->dir + angle;

            longueur = randomBetween(0.7, 1.3);
        }
        direction = clampDirection(direction);
        b->p_fils[i] = growBranch(a, b, saison, direction, longueur);
    }
    b->nb_fils = n;
    b->etat = BOIS;
}

void growTree(t_arbre *a, int nbSaisons)
{
    t_queue F = createEmptyQueue();
    enqueue(&F, a->pied);

    for (int saison = 1; saison <= nbSaisons; saison++)
    {
        t_queue G = createEmptyQueue();

        while (!isEmptyQueue(F))
        {
            t_rameau *b = dequeue(&F);

            if (canGrow(95))
            {
                int n = howManyBranches(70, 25);
                growBud(a, b, n, saison);

                for (int i = 0; i < b->nb_fils; i++)
                    enqueue(&G, b->p_fils[i]);
            }
            else
            {
                enqueue(&G, b);
            }
        }
        F = G;
    }

    while (!isEmptyQueue(F))
        dequeue(&F);
}