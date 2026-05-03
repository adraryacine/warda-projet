#include "structures.h"

/* Retourne l'heure actuelle sous forme "HH:MM:SS" */
void get_heure_actuelle(char *buf, int taille) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, taille, "%H:%M:%S", tm_info);
}

/* Chaîne lisible pour la couleur */
const char* couleur_str(int c) {
    switch (c) {
        case ROUGE: return "ROUGE (Critique)";
        case JAUNE: return "JAUNE (Urgent)";
        case VERT:  return "VERT  (Non urgent)";
        default:    return "INCONNUE";
    }
}

/* Chaîne lisible pour le statut */
const char* statut_str(int s) {
    switch (s) {
        
        case EN_ATTENTE_INFIRMIER:  return "En attente infirmier";
        case EN_ATTENTE_MEDECIN:    return "En attente medecin";
        case EN_CONSULTATION:       return "En consultation";
        case SOUS_OBSERVATION:      return "Sous observation";
        case SORTI_ORDONNANCE:      return "Sorti (ordonnance)";
        case HOSPITALISE:           return "Hospitalise";
        case REFERE:                return "Refere";
        case CATASTROPHE_VICTIME:   return "Victime catastrophe";
        default:                    return "Inconnu";
    }
}

/* Crée un nouveau patient et lui assigne un ticket */
Patient* creer_patient(Systeme *sys, const char *nom, const char *prenom,
                        const char *id, int age) {
    Patient *p = (Patient*)malloc(sizeof(Patient));
    if (!p) {
        fprintf(stderr, "Erreur allocation patient.\n");
        return NULL;
    }
    memset(p, 0, sizeof(Patient));

    sys->compteur_ticket++;
    p->ticket = sys->compteur_ticket;

    strncpy(p->nom,    nom,    MAX_NOM    - 1);
    strncpy(p->prenom, prenom, MAX_PRENOM - 1);
    strncpy(p->id,     id,     MAX_ID     - 1);
    p->age    = age;
    p->statut = EN_ATTENTE_INFIRMIER;  /* attend le triage infirmier */
    p->couleur = VERT;                 /* défaut avant triage */
    p->lit    = -1;
    p->suivant = NULL;

    get_heure_actuelle(p->heure_arrivee, sizeof(p->heure_arrivee));

    sys->stat_total++;
    return p;
}

/* Affiche les informations d'un patient */
void afficher_patient(const Patient *p) {
    if (!p) return;
    printf("  Ticket    : #%03d\n", p->ticket);
    printf("  ID        : %s\n",    p->id);
    printf("  Nom       : %s %s\n", p->prenom, p->nom);
    printf("  Age       : %d ans\n",p->age);
    printf("  Arrivee   : %s\n",    p->heure_arrivee);

    /* Affiche couleur avec ANSI */
    printf("  Priorite  : ");
    afficher_couleur_ansi(p->couleur);
    printf("%s", couleur_str(p->couleur));
    reset_couleur_ansi();
    printf("\n");

    if (strlen(p->etat) > 0)
        printf("  Etat      : %s\n", p->etat);
    printf("  Statut    : %s\n", statut_str(p->statut));
    if (strlen(p->medecin_assigne) > 0)
        printf("  Medecin   : Dr. %s\n", p->medecin_assigne);
    if (strlen(p->ordonnance) > 0)
        printf("  Ordonnance: %s\n", p->ordonnance);
    if (strlen(p->departement_ref) > 0)
        printf("  Ref. dept : %s\n", p->departement_ref);
    if (p->lit >= 0)
        printf("  Lit n°    : %d\n", p->lit);
}
