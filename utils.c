#include "structures.h"

/* Vide le buffer stdin après une saisie */
void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Lit un entier dans [min, max] avec message */
int lire_entier(const char *msg, int min, int max) {
    int val;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &val) == 1) {
            vider_buffer();
            if (val >= min && val <= max) return val;
        } else {
            vider_buffer();
        }
        printf("  -> Valeur invalide. Entrez un nombre entre %d et %d.\n", min, max);
    }
}

/* Lit une chaîne non vide */
void lire_chaine(const char *msg, char *dest, int taille) {
    while (1) {
        printf("%s", msg);
        if (fgets(dest, taille, stdin)) {
            /* retirer le \n final */
            int len = strlen(dest);
            if (len > 0 && dest[len-1] == '\n') dest[len-1] = '\0';
            if (strlen(dest) > 0) return;
        }
        printf("  -> Champ obligatoire.\n");
    }
}

/* Affiche une ligne de séparateur */
void afficher_separateur(char c, int n) {
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

/* Affiche un titre encadré */
void afficher_titre(const char *titre) {
    int len = strlen(titre) + 4;
    afficher_separateur('=', len);
    printf("| %s |\n", titre);
    afficher_separateur('=', len);
}

/* Couleur ANSI selon triage */
void afficher_couleur_ansi(int c) {
    switch (c) {
        case ROUGE: printf("\033[1;31m"); break; /* rouge gras */
        case JAUNE: printf("\033[1;33m"); break; /* jaune gras */
        case VERT:  printf("\033[1;32m"); break; /* vert gras  */
    }
}

void reset_couleur_ansi(void) {
    printf("\033[0m");
}
