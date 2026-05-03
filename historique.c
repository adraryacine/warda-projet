#include "structures.h"

void hist_init(Historique *h) {
    h->tete   = NULL;
    h->taille = 0;
}

/* Ajoute un patient à l'historique */
void hist_ajouter(Historique *h, Patient *p) {
    NoeudHist *noeud = (NoeudHist*)malloc(sizeof(NoeudHist));
    if (!noeud) return;
    noeud->patient  = p;
    noeud->suivant  = h->tete;
    h->tete         = noeud;
    h->taille++;
}

/* Affiche l'historique complet */
void afficher_historique(const Historique *h) {
    if (!h->tete) {
        printf("  [ Historique vide ]\n");
        return;
    }
    printf("  Historique - %d patients traites\n", h->taille);
    afficher_separateur('-', 70);
    printf("  %-6s %-15s %-15s %-5s %-20s %-20s\n",
           "Ticket","Nom","Prenom","Age","Statut final","Medecin");
    afficher_separateur('-', 70);

    NoeudHist *courant = h->tete;
    while (courant) {
        Patient *p = courant->patient;
        printf("  #%-5d %-15s %-15s %-5d %-20s %-20s\n",
               p->ticket, p->nom, p->prenom, p->age,
               statut_str(p->statut),
               strlen(p->medecin_assigne) > 0 ? p->medecin_assigne : "-");
        courant = courant->suivant;
    }
    afficher_separateur('-', 70);
}
