#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "statistiques.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage : %s fichier colonne [nombre_de_classes [fichier_svg]]\n", argv[0]);
        return EXIT_FAILURE;
    }
    int nb_classes = argc >= 4 ? atoi(argv[3]) : 20;
    const char *svg = argc >= 5 ? argv[4] : "output/histogramme.svg";
    if (nb_classes <= 0 || nb_classes > 1000) {
        fprintf(stderr, "Nombre de classes invalide.\n");
        return EXIT_FAILURE;
    }
    int n = 0;
    double *v = readColumn(argv[1], argv[2], &n);
    if (!v || n <= 0) {
        fprintf(stderr, "Pas de valeurs pour %s dans %s.\n", argv[2], argv[1]);
        free(v);
        return EXIT_FAILURE;
    }
    displaySummary(v, n);
    double minimum = 0, largeur = 0;
    int *effectifs = computeHistogram(v, n, nb_classes, &minimum, &largeur);
    if (!effectifs) { free(v); return EXIT_FAILURE; }
    printf("minimum %.4f, largeur d'une classe %.4f\n", minimum, largeur);
    displayHistogram(effectifs, nb_classes, minimum, largeur);
    histoToSVG(effectifs, nb_classes, minimum, largeur, argv[2], svg);
    printf("dessin : %s\n", svg);
    free(effectifs);
    free(v);
    return EXIT_SUCCESS;
}
