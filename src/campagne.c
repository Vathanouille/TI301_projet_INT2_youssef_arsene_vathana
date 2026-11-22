#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "campagne.h"
#include "foret.h"

t_campagne createCampaign(void)
{
    t_campagne c;
    c.K = 50;
    c.E = 3;
    c.A = 0.25;
    c.S = 120;
    c.R = 1;
    c.graine = 1;
    c.identique = 0;
    strcpy(c.sortie, "output/campagne");
    return c;
}

int runCampaign(t_campagne c)
{
    char nom_arbres[LONGUEUR_NOM + 32];
    char nom_forets[LONGUEUR_NOM + 32];
    snprintf(nom_arbres, sizeof(nom_arbres), "%s_arbres.csv", c.sortie);
    snprintf(nom_forets, sizeof(nom_forets), "%s_forets.csv", c.sortie);

    FILE *arbres = fopen(nom_arbres, "wt");
    if (!arbres) { perror(nom_arbres); return 0; }
    FILE *forets = fopen(nom_forets, "wt");
    if (!forets) { perror(nom_forets); fclose(arbres); return 0; }

    /* Les sorties doivent conserver le point décimal, indépendamment de la langue du PC. */
    setlocale(LC_NUMERIC, "C");
    fprintf(arbres, "# K=%d E=%d A=%.3f S=%d R=%d graine=%d identique=%d HMAX=%d P_max=%d\n",
            c.K, c.E, c.A, c.S, c.R, c.graine, c.identique, HMAX_CAMPAGNE, PMAX_CAMPAGNE);
    fprintf(forets, "# K=%d E=%d A=%.3f S=%d R=%d graine=%d identique=%d HMAX=%d P_max=%d\n",
            c.K, c.E, c.A, c.S, c.R, c.graine, c.identique, HMAX_CAMPAGNE, PMAX_CAMPAGNE);
    fprintf(arbres, "foret;arbre;rameaux;hauteur;cime;extension;tronque\n");
    fprintf(forets, "foret;rameaux;cime_moy;extension_moy\n");

    clock_t debut = clock();
    for (int r = 0; r < c.R; ++r) {
        t_foret f = createForest(c.K, c.E, HMAX_CAMPAGNE, c.A, PMAX_CAMPAGNE);
        plantForest(&f, c.graine + r, c.identique);
        simulateForest(&f, c.S);
        long total = 0;
        double somme_cimes = 0.0, somme_extensions = 0.0;
        for (int i = 0; i < f.K; ++i) {
            t_arbre_foret *a = &f.arbres[i];
            float extension = a->xmax - a->xmin;
            total += a->step.arbre.nb_rameaux;
            somme_cimes += a->cime;
            somme_extensions += extension;
            fprintf(arbres, "%d;%d;%d;%d;%.3f;%.3f;%d\n", r, a->id,
                    a->step.arbre.nb_rameaux, a->hauteur, a->cime, extension, a->tronque);
        }
        fprintf(forets, "%d;%ld;%.4f;%.4f\n", r, total,
                somme_cimes / f.K, somme_extensions / f.K);
        printf("Foret %d/%d (graine %d) : %ld rameaux, cime moyenne %.4f, extension moyenne %.4f\n",
               r + 1, c.R, c.graine + r, total, somme_cimes / f.K, somme_extensions / f.K);
        freeForest(&f);
    }
    printf("Temps de calcul total : %.3f secondes\n", (double)(clock() - debut) / CLOCKS_PER_SEC);
    fclose(arbres);
    fclose(forets);
    return 1;
}
