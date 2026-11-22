#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "campagne.h"

static void usage(const char *programme)
{
    fprintf(stderr, "Usage : %s [--K entier] [--E entier] [--A reel] [--S entier] "
                    "[--identique] [--R entier] [--graine entier] [--sortie prefixe]\n", programme);
}

int main(int argc, char *argv[])
{
    t_campagne c = createCampaign();
    for (int i = 1; i < argc; ++i) {
        const char *option = argv[i];
        if (strcmp(option, "--identique") == 0) {
            c.identique = 1;
            continue;
        }
        if (i + 1 >= argc || (argv[i+1][0] == '-' && argv[i+1][1] == '-')) {
            fprintf(stderr, "Valeur manquante ou parametre inconnu : %s\n", option);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        const char *valeur = argv[++i];
        if (strcmp(option, "--K") == 0) c.K = atoi(valeur);
        else if (strcmp(option, "--E") == 0) c.E = atoi(valeur);
        else if (strcmp(option, "--A") == 0) c.A = atof(valeur);
        else if (strcmp(option, "--S") == 0) c.S = atoi(valeur);
        else if (strcmp(option, "--R") == 0) c.R = atoi(valeur);
        else if (strcmp(option, "--graine") == 0) c.graine = atoi(valeur);
        else if (strcmp(option, "--sortie") == 0) {
            if (strlen(valeur) >= sizeof(c.sortie)) {
                fprintf(stderr, "Nom de sortie trop long.\n");
                return EXIT_FAILURE;
            }
            strcpy(c.sortie, valeur);
        } else {
            fprintf(stderr, "Parametre inconnu : %s\n", option);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (c.K <= 0 || c.E <= 0 || c.S < 0 || c.R <= 0 || c.A < 0 || c.A > 1) {
        fprintf(stderr, "Parametres invalides (K,E,R > 0 ; S >= 0 ; 0 <= A <= 1).\n");
        return EXIT_FAILURE;
    }
    return runCampaign(c) ? EXIT_SUCCESS : EXIT_FAILURE;
}
