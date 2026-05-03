#include "structures.h"

void obs_init(ListeObservation *lo) {
    lo->tete   = NULL;
    lo->taille = 0;
    memset(lo->lits_occupes, 0, sizeof(lo->lits_occupes));
}

/* Trouve un lit libre (numéros 1..20) */
static int trouver_lit_libre(ListeObservation *lo) {
    for (int i = 0; i < 20; i++) {
        if (lo->lits_occupes[i] == 0) {
            lo->lits_occupes[i] = 1;
            return i + 1; /* lit numéroté à partir de 1 */
        }
    }
    return -1; /* plus de lits disponibles */
}

/* Ajoute un patient en observation */
int obs_ajouter(ListeObservation *lo, Patient *p) {
    int lit = trouver_lit_libre(lo);
    if (lit == -1) {
        printf("  ERREUR : Aucun lit disponible en observation !\n");
        return 0;
    }

    NoeudObs *noeud = (NoeudObs*)malloc(sizeof(NoeudObs));
    if (!noeud) return 0;

    p->lit    = lit;
    p->statut = SOUS_OBSERVATION;

    noeud->patient  = p;
    noeud->suivant  = lo->tete;
    lo->tete        = noeud;
    lo->taille++;
    return 1;
}

/* Retire un patient de l'observation par son ticket */
Patient* obs_retirer(ListeObservation *lo, int ticket) {
    NoeudObs *courant = lo->tete;
    NoeudObs *prec    = NULL;

    while (courant) {
        if (courant->patient->ticket == ticket) {
            if (prec) prec->suivant    = courant->suivant;
            else      lo->tete         = courant->suivant;

            Patient *p = courant->patient;
            /* libérer le lit */
            if (p->lit >= 1 && p->lit <= 20)
                lo->lits_occupes[p->lit - 1] = 0;

            free(courant);
            lo->taille--;
            return p;
        }
        prec    = courant;
        courant = courant->suivant;
    }
    return NULL; /* non trouvé */
}

/* Affiche la liste des patients sous observation */
void afficher_observation(const ListeObservation *lo) {
    if (!lo->tete) {
        printf("  [ Aucun patient sous observation ]\n");
        return;
    }
    printf("  Patients sous observation : %d\n", lo->taille);
    afficher_separateur('-', 60);

    NoeudObs *courant = lo->tete;
    while (courant) {
        Patient *p = courant->patient;
        printf("  Lit %2d | Ticket #%03d | %s %s | Age %d | ",
               p->lit, p->ticket, p->prenom, p->nom, p->age);
        afficher_couleur_ansi(p->couleur);
        printf("%s", couleur_str(p->couleur));
        reset_couleur_ansi();
        printf("\n");
        if (strlen(p->etat) > 0)
            printf("         Etat : %s\n", p->etat);
        courant = courant->suivant;
    }
}
