#include "structures.h"

/* Initialise les médecins (saisie interactive) */
void init_medecins(Systeme *sys) {
    sys->nb_medecins = 0;
    afficher_titre("ENREGISTREMENT DES MEDECINS");
    int n = lire_entier("Combien de medecins aujourd'hui ? (1-10) : ", 1, MAX_MEDECINS);
    sys->nb_medecins = n;
    for (int i = 0; i < n; i++) {
        char buf[MAX_NOM];
        printf("\n  Medecin %d :\n", i+1);
        lire_chaine("    Nom du medecin : Dr. ", buf, MAX_NOM);
        strncpy(sys->medecins[i].nom, buf, MAX_NOM - 1);
        sys->medecins[i].statut           = DISPONIBLE;
        sys->medecins[i].patients_traites = 0;
        printf("    -> Dr. %s enregistre comme DISPONIBLE.\n", buf);
    }
}

/* Affiche l'état de chaque médecin (style Idée 14) */
void afficher_medecins(const Systeme *sys) {
    afficher_separateur('-', 50);
    printf("  MEDECINS DE GARDE\n");
    afficher_separateur('-', 50);
    for (int i = 0; i < sys->nb_medecins; i++) {
        printf("  Dr. %-20s : ", sys->medecins[i].nom);
        if (sys->medecins[i].statut == DISPONIBLE) {
            printf("\033[1;32m[DISPONIBLE]    \033[0m");
        } else {
            printf("\033[1;31m[NON DISPONIBLE]\033[0m");
        }
        printf(" | Patients traites : %d\n",
               sys->medecins[i].patients_traites);
    }
    afficher_separateur('-', 50);
}

/* Retourne l'index du premier médecin disponible, -1 sinon */
int trouver_medecin_dispo(const Systeme *sys) {
    for (int i = 0; i < sys->nb_medecins; i++) {
        if (sys->medecins[i].statut == DISPONIBLE) return i;
    }
    return -1;
}

/* Assigne un patient à un médecin */
void medecin_prend_patient(Systeme *sys, int idx, Patient *p) {
    sys->medecins[idx].statut = NON_DISPONIBLE;
    strncpy(p->medecin_assigne, sys->medecins[idx].nom, MAX_NOM - 1);
    p->statut = EN_CONSULTATION;
}

/* Libère un médecin après consultation */
void medecin_libere(Systeme *sys, int idx) {
    sys->medecins[idx].statut = DISPONIBLE;
    sys->medecins[idx].patients_traites++;
}

/* Trouve l'index d'un médecin par son nom */
int medecin_index_par_nom(const Systeme *sys, const char *nom) {
    for (int i = 0; i < sys->nb_medecins; i++) {
        if (strcmp(sys->medecins[i].nom, nom) == 0) return i;
    }
    return -1;
}
