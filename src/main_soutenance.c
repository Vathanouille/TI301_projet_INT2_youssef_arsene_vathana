/*
 * main_soutenance.c - TI301 - Project defence
 *
 * Demonstrates parts 1, 2 and 3 of the forest project, using the existing
 * functions implemented in the project. The settings below can be changed
 * quickly during the defence.
 *
 * Run the CMake target "soutenance" from the PROJECT ROOT.
 * Optional command-line argument: 1, 2 or 3 to run just that part.
 * Without an argument, the program runs all three parts.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

/* Part 1: individual trees */
#include "arbre.h"
#include "croissance.h"
#include "croissance_step.h"
#include "sauver_arbre.h"

/* Part 2: forest and light */
#include "arbre_foret.h"
#include "foret.h"
#include "sauver_foret.h"

/* Part 3: campaigns and statistics */
#include "campagne.h"
#include "statistiques.h"

/* ===== SETTINGS TO CHANGE DURING THE DEFENCE =========================== */
#define TREE_SEED            1
#define TREE_SEASONS         5
#define FOREST_SEED          1
#define FOREST_K             24
#define FOREST_E             3
#define FOREST_HMAX          160
#define FOREST_SEASONS       32
#define FOREST_A             0.25
#define FOREST_P_MAX         20000
#define CAMPAIGN_R           3
#define CAMPAIGN_K           24
#define CAMPAIGN_SEASONS     32
#define HISTOGRAM_BINS       10
/* ====================================================================== */

/* The examples save files to output/. This works on Windows and on Linux. */
static int ensureOutputDirectory(void)
{
#ifdef _WIN32
    int result = _mkdir("output");
#else
    int result = mkdir("output", 0777);
#endif
    if (result != 0 && errno != EEXIST) {
        perror("Cannot create output directory");
        return 0;
    }
    return 1;
}

static void demonstratePart1(void)
{
    printf("\n========== PART 1: SINGLE TREE ==========\n");

    /* Grow a tree for a fixed number of seasons and save it. */
    t_arbre a = createTree(0.0f, 1.0f, TREE_SEED);
    growTree(&a, TREE_SEASONS);
    printf("Single tree: %d branches after %d seasons\n",
           a.nb_rameaux, TREE_SEASONS);
    saveTree(a, "output/tree_defence.txt");
    freeBranches(a.pied);

    /* Demonstrate seasonal growth with the bud queue. */
    printf("\nSeason-by-season growth:\n");
    t_arbre_step s = createTreeStep(0.0f, 1.0f, TREE_SEED);
    for (int i = 0; i < TREE_SEASONS; i++) {
        growTreeStep(&s);
        printf("Season %d: %d branches\n", s.saison, s.arbre.nb_rameaux);
    }
    clearBuds(&s);
    freeBranches(s.arbre.pied);
    printf("Saved: output/tree_defence.txt\n");
}

static void demonstratePart2(void)
{
    printf("\n========== PART 2: FOREST AND LIGHT ==========\n");

    /* Single forest tree: one season in full light. */
    t_arbre_foret af = createForestTree(0, 36.0f, 1.0f, FOREST_SEED);
    growSeason(&af, 1.0);
    printf("Forest tree after one season:\n");
    printf("  Canopy (top): %.2f\n", af.cime);
    /* The extent is the difference between the extreme x coordinates. */
    printf("  Extent      : %.2f\n", af.xmax - af.xmin);
    printf("  Branches    : %d\n", af.step.arbre.nb_rameaux);
    freeForestTree(&af);

    /* Full forest: growth over several seasons with shading. */
    t_foret f = createForest(FOREST_K, FOREST_E, FOREST_HMAX,
                            FOREST_A, FOREST_P_MAX);
    plantForest(&f, FOREST_SEED, 1); /* 1: identical initial trunk heights */
    simulateForest(&f, FOREST_SEASONS);

    long total = 0;
    for (int i = 0; i < f.K; i++) {
        printf("Tree %2d: %5d branches, canopy = %.2f\n",
               i, f.arbres[i].step.arbre.nb_rameaux, f.arbres[i].cime);
        total += f.arbres[i].step.arbre.nb_rameaux;
    }
    printf("Forest total after %d seasons: %ld branches\n",
           FOREST_SEASONS, total);
    saveForest(&f, "output/forest_defence.txt");
    freeForest(&f);
    printf("Saved: output/forest_defence.txt\n");
}

static int demonstratePart3(void)
{
    printf("\n========== PART 3: CAMPAIGN AND STATISTICS ==========\n");

    /* A deliberately short campaign for a live demonstration. */
    t_campagne c = createCampaign();
    c.K = CAMPAIGN_K;
    c.E = FOREST_E;
    c.A = FOREST_A;
    c.S = CAMPAIGN_SEASONS;
    c.R = CAMPAIGN_R;
    c.graine = FOREST_SEED;
    c.identique = 0; /* 0: random trunk heights */
    snprintf(c.sortie, LONGUEUR_NOM, "output/defence");

    printf("Campaign: %d forests, %d trees each, %d seasons\n",
           c.R, c.K, c.S);
    if (!runCampaign(c)) {
        fprintf(stderr, "The simulation campaign failed.\n");
        return 0;
    }
    printf("Saved: output/defence_arbres.csv and output/defence_forets.csv\n");

    /* Read the example CSV supplied with the project and draw its histogram. */
    int n = 0;
    double *vals = readColumn("data/test_histo.csv", "note", &n);
    if (vals == NULL || n <= 0) {
        fprintf(stderr,
                "Cannot read data/test_histo.csv (column 'note'). "
                "Run from the project's root directory.\n");
        free(vals);
        return 0;
    }

    printf("\nSummary of the 'note' column (%d values):\n", n);
    displaySummary(vals, n);

    double min_val = 0.0, width = 0.0;
    int *counts = computeHistogram(vals, n, HISTOGRAM_BINS,
                                   &min_val, &width);
    if (counts == NULL) {
        fprintf(stderr, "Cannot compute histogram.\n");
        free(vals);
        return 0;
    }
    displayHistogram(counts, HISTOGRAM_BINS, min_val, width);
    histoToSVG(counts, HISTOGRAM_BINS, min_val, width,
               "Grade Distribution", "output/defence_histo.svg");
    printf("Saved: output/defence_histo.svg\n");

    free(counts);
    free(vals);
    return 1;
}

int main(int argc, char *argv[])
{
    int part = 0; /* 0 = all parts */
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "1") != 0 &&
                     strcmp(argv[1], "2") != 0 && strcmp(argv[1], "3") != 0)) {
        fprintf(stderr, "Usage: %s [1|2|3]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        part = atoi(argv[1]);
    }

    if (!ensureOutputDirectory()) {
        return EXIT_FAILURE;
    }
    printf("=== TI301 Project Defence - The Forest and the Light ===\n");
    printf("Change parameters in the SETTINGS section of main_soutenance.c.\n");

    if (part == 0 || part == 1) {
        demonstratePart1();
    }
    if (part == 0 || part == 2) {
        demonstratePart2();
    }
    if (part == 0 || part == 3) {
        if (!demonstratePart3()) {
            return EXIT_FAILURE;
        }
    }

    printf("\n=== End of Defence Execution ===\n");
    return EXIT_SUCCESS;
}




// Created by Vathana on 24/11/2026.
//