#include "structures.h"

/* Active le mode catastrophe (Mass Casualty) - Idée 6 */
void activer_mode_catastrophe(Systeme *sys) {
    if (sys->mode_catastrophe) {
        printf("  Mode catastrophe deja actif !\n");
        return;
    }

    printf("\n");
    afficher_separateur('!', 60);
    printf("  *** MODE CATASTROPHE ACTIVE ***\n");
    afficher_separateur('!', 60);

    int nb_victimes = lire_entier("  Nombre de victimes : ", 1, 100);
    sys->nb_catastrophe_victimes += nb_victimes;
    sys->mode_catastrophe = 1;

    char cause[100];
    lire_chaine("  Cause (ex: Accident de route) : ", cause, 100);

    printf("\n  Actions automatiques :\n");
    printf("  -> Tri automatique par gravite\n");
    printf("  -> Reservation lits d'urgence\n");
    printf("  -> Blocage des cas non urgents (VERT suspendus)\n");
    printf("  -> Notification tous departements\n\n");

    /* Suspendre les patients VERT en file - les mettre "en attente" */
    Patient *courant = sys->file_attente.tete;
    int suspendus = 0;
    while (courant) {
        if (courant->couleur == VERT) suspendus++;
        courant = courant->suivant;
    }
    if (suspendus > 0)
        printf("  -> %d patient(s) VERT mis en attente prolongee\n", suspendus);

    /* Enregistrer les victimes directement en ROUGE */
    printf("\n  Enregistrement des %d victimes :\n", nb_victimes);
    for (int i = 0; i < nb_victimes; i++) {
        Patient *v = (Patient*)malloc(sizeof(Patient));
        if (!v) continue;
        memset(v, 0, sizeof(Patient));

        sys->compteur_ticket++;
        v->ticket = sys->compteur_ticket;
        sys->stat_total++;

        snprintf(v->nom,    MAX_NOM,    "Victime_%d", i+1);
        snprintf(v->prenom, MAX_PRENOM, "%.*s", (int)(MAX_PRENOM - 1), cause);
        snprintf(v->id,     MAX_ID,     "CAT%03d", i+1);
        v->age    = 0; /* inconnu */
        v->couleur = ROUGE;
        v->statut  = CATASTROPHE_VICTIME;
        v->lit     = -1;
        get_heure_actuelle(v->heure_arrivee, sizeof(v->heure_arrivee));
        snprintf(v->etat, MAX_ETAT, "Victime catastrophe - %s", cause);

        file_enqueue_priorite(&sys->file_attente, v);
        printf("    Victime %d -> Ticket #%03d [ROUGE]\n", i+1, v->ticket);
    }

    afficher_separateur('!', 60);
    printf("  MODE CATASTROPHE : %d victimes enregistrees en priorite ROUGE\n",
           nb_victimes);
    afficher_separateur('!', 60);
}

/* Désactive le mode catastrophe */
void desactiver_mode_catastrophe(Systeme *sys) {
    if (!sys->mode_catastrophe) {
        printf("  Le mode catastrophe n'est pas actif.\n");
        return;
    }
    sys->mode_catastrophe = 0;
    printf("  -> Mode catastrophe DESACTIVE. Retour au fonctionnement normal.\n");
}
