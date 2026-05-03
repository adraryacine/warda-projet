#include "structures.h"

/* Initialise la file */
void file_init(FileAttente *f) {
    f->tete   = NULL;
    f->queue  = NULL;
    f->taille = 0;
}

/*
 * Enfile un patient selon sa priorité :
 * ROUGE (1) > JAUNE (2) > VERT (3)
 * Les ROUGE passent devant tout le monde,
 * Les JAUNE passent devant les VERT,
 * Les VERT s'ajoutent à la fin.
 * À priorité égale : ordre FIFO (arrivée).
 */
void file_enqueue_priorite(FileAttente *f, Patient *p) {
    p->suivant = NULL;

    /* File vide */
    if (f->tete == NULL) {
        f->tete  = p;
        f->queue = p;
        f->taille++;
        return;
    }

    /* Insérer devant tous ceux de priorité INFÉRIEURE */
    if (p->couleur < f->tete->couleur) {
        /* Passe en tête absolue */
        p->suivant = f->tete;
        f->tete    = p;
        f->taille++;
        return;
    }

    /* Trouver la bonne position */
    Patient *courant = f->tete;
    while (courant->suivant != NULL &&
           courant->suivant->couleur <= p->couleur) {
        courant = courant->suivant;
    }

    p->suivant       = courant->suivant;
    courant->suivant = p;
    if (p->suivant == NULL) f->queue = p;
    f->taille++;
}

/* Défile le premier patient (priorité la plus haute) */
Patient* file_dequeue(FileAttente *f) {
    if (f->tete == NULL) return NULL;
    Patient *p = f->tete;
    f->tete = p->suivant;
    if (f->tete == NULL) f->queue = NULL;
    p->suivant = NULL;
    f->taille--;
    return p;
}

/* Défile spécifiquement un ROUGE (mode catastrophe ou urgence absolue) */
Patient* file_dequeue_rouge(FileAttente *f) {
    if (f->tete == NULL) return NULL;
    if (f->tete->couleur != ROUGE) return NULL;
    return file_dequeue(f);
}

int file_est_vide(const FileAttente *f) {
    return (f->tete == NULL);
}

int file_taille(const FileAttente *f) {
    return f->taille;
}

/* Affiche la file d'attente triée */
void afficher_file(const FileAttente *f) {
    if (file_est_vide(f)) {
        printf("  [ File d'attente vide ]\n");
        return;
    }
    printf("  Nombre de patients en attente : %d\n", f->taille);
    afficher_separateur('-', 60);

    Patient *courant = f->tete;
    int pos = 1;
    while (courant) {
        printf("  [%2d] Ticket #%03d | %-20s %-15s | ",
               pos, courant->ticket,
               courant->nom, courant->prenom);
        afficher_couleur_ansi(courant->couleur);
        printf("%-18s", couleur_str(courant->couleur));
        reset_couleur_ansi();
        printf("\n");
        courant = courant->suivant;
        pos++;
    }
}
