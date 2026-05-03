#include "structures.h"

int main(void) {
    /* ---- Initialisation du système ---- */
    Systeme sys;
    memset(&sys, 0, sizeof(Systeme));

    file_init(&sys.file_attente);
    obs_init(&sys.observation);
    hist_init(&sys.historique);

    sys.compteur_ticket       = 0;
    sys.mode_catastrophe      = 0;
    sys.nb_catastrophe_victimes = 0;
    sys.stat_total            = 0;
    sys.stat_discharges       = 0;
    sys.stat_hospitalises     = 0;
    sys.stat_referes          = 0;

    /* ---- Écran d'accueil ---- */
    printf("\n");
    afficher_separateur('*', 55);
    printf("*  BIENVENUE AU SYSTEME DE GESTION DES URGENCES   *\n");
    printf("*     Universite A.Mira - Bejaia  |  DSA-2        *\n");
    afficher_separateur('*', 55);
    printf("\n");

    /* ---- Charger l'historique précédent s'il existe ---- */
    charger_historique(&sys.historique, "historique.txt");
    if (sys.historique.taille > 0)
        printf("  %d patient(s) charge(s) depuis l'historique.\n\n",
               sys.historique.taille);

    /* ---- Enregistrement des médecins de garde ---- */
    init_medecins(&sys);

    /* ---- Boucle principale ---- */
    menu_principal(&sys);

    /* ---- Sauvegarde automatique à la fermeture ---- */
    printf("\n  Sauvegarde automatique...\n");
    sauvegarder_historique(&sys.historique, "historique.txt");
    sauvegarder_rapport_journalier(&sys, "rapport_final.txt");
    printf("  Au revoir !\n\n");

    /* ---- Libération mémoire (bonne pratique) ---- */
    /* File d'attente */
    Patient *tmp;
    while (!file_est_vide(&sys.file_attente)) {
        tmp = file_dequeue(&sys.file_attente);
        free(tmp);
    }
    /* Observation */
    NoeudObs *no = sys.observation.tete;
    while (no) {
        NoeudObs *next = no->suivant;
        free(no->patient);
        free(no);
        no = next;
    }
    /* Historique */
    NoeudHist *nh = sys.historique.tete;
    while (nh) {
        NoeudHist *next = nh->suivant;
        free(nh->patient);
        free(nh);
        nh = next;
    }

    return 0;
}
