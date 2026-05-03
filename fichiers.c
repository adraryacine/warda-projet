#include "structures.h"

/* Sauvegarde l'historique dans un fichier texte */
void sauvegarder_historique(const Historique *h, const char *fichier) {
    FILE *f = fopen(fichier, "w");
    if (!f) {
        printf("  ERREUR : Impossible d'ouvrir %s\n", fichier);
        return;
    }

    fprintf(f, "HISTORIQUE DES PATIENTS\n");
    fprintf(f, "========================\n\n");

    NoeudHist *courant = h->tete;
    while (courant) {
        Patient *p = courant->patient;
        fprintf(f, "Ticket     : #%03d\n", p->ticket);
        fprintf(f, "ID         : %s\n",    p->id);
        fprintf(f, "Nom        : %s %s\n", p->prenom, p->nom);
        fprintf(f, "Age        : %d ans\n",p->age);
        fprintf(f, "Arrivee    : %s\n",    p->heure_arrivee);
        fprintf(f, "Priorite   : %s\n",    couleur_str(p->couleur));
        fprintf(f, "Etat       : %s\n",    p->etat);
        fprintf(f, "Statut     : %s\n",    statut_str(p->statut));
        if (strlen(p->medecin_assigne) > 0)
            fprintf(f, "Medecin    : Dr. %s\n", p->medecin_assigne);
        if (strlen(p->ordonnance) > 0)
            fprintf(f, "Ordonnance : %s\n", p->ordonnance);
        if (strlen(p->departement_ref) > 0)
            fprintf(f, "Ref. dept  : %s\n", p->departement_ref);
        fprintf(f, "---\n");
        courant = courant->suivant;
    }

    fclose(f);
    printf("  -> Historique sauvegarde dans '%s'\n", fichier);
}

/* Génère le rapport journalier style Idée 7 */
void sauvegarder_rapport_journalier(const Systeme *sys, const char *fichier) {
    FILE *f = fopen(fichier, "w");
    if (!f) {
        printf("  ERREUR : Impossible de creer le rapport.\n");
        return;
    }

    /* Obtenir la date */
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_buf[20];
    strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", tm_info);

    fprintf(f, "==================================================\n");
    fprintf(f, "       RAPPORT JOURNALIER - %s\n", date_buf);
    fprintf(f, "==================================================\n\n");
    fprintf(f, "  Total patients        : %d\n", sys->stat_total);
    fprintf(f, "  Discharges            : %d\n", sys->stat_discharges);
    fprintf(f, "  Hospitalises          : %d\n", sys->stat_hospitalises);
    fprintf(f, "  Referes               : %d\n", sys->stat_referes);
    fprintf(f, "  En attente (actuel)   : %d\n", sys->file_attente.taille);
    fprintf(f, "  En observation        : %d\n", sys->observation.taille);
    fprintf(f, "\n");

    fprintf(f, "--------------------------------------------------\n");
    fprintf(f, "  MEDECINS\n");
    fprintf(f, "--------------------------------------------------\n");
    for (int i = 0; i < sys->nb_medecins; i++) {
        fprintf(f, "  Dr. %-20s : %d patients traites\n",
                sys->medecins[i].nom,
                sys->medecins[i].patients_traites);
    }
    fprintf(f, "\n==================================================\n");
    fprintf(f, "  Mode catastrophe : %s\n",
            sys->mode_catastrophe ? "OUI (ACTIVE)" : "Non");
    if (sys->mode_catastrophe)
        fprintf(f, "  Victimes catastrophe : %d\n", sys->nb_catastrophe_victimes);
    fprintf(f, "==================================================\n");

    fclose(f);
    printf("  -> Rapport sauvegarde dans '%s'\n", fichier);
}

/* Charge l'historique depuis un fichier (format simplifié) */
void charger_historique(Historique *h, const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (!f) return; /* pas de fichier existant, normal au 1er lancement */

    /* lecture basique : on recrée des patients depuis le fichier */
    char ligne[300];
    Patient *p = NULL;

    while (fgets(ligne, sizeof(ligne), f)) {
        /* Nouveau patient si on voit "Ticket" */
        if (strncmp(ligne, "Ticket", 6) == 0) {
            p = (Patient*)malloc(sizeof(Patient));
            if (!p) continue;
            memset(p, 0, sizeof(Patient));
            p->lit = -1;
            sscanf(ligne, "Ticket     : #%d", &p->ticket);
        }
        if (!p) continue;

        if (strncmp(ligne, "Nom", 3) == 0)
            sscanf(ligne, "Nom        : %49s %49s", p->prenom, p->nom);
        else if (strncmp(ligne, "Age", 3) == 0)
            sscanf(ligne, "Age        : %d", &p->age);
        else if (strncmp(ligne, "ID", 2) == 0)
            sscanf(ligne, "ID         : %19s", p->id);
        else if (strncmp(ligne, "---", 3) == 0 && p) {
            hist_ajouter(h, p);
            p = NULL;
        }
    }
    if (p) { hist_ajouter(h, p); } /* dernier patient sans "---" */
    fclose(f);
}
