#include "structures.h"

/*ENREGISTREMENT D'UN NOUVEAU PATIENT (réception)*/
void menu_enregistrer_patient(Systeme *sys) {
    afficher_titre("ENREGISTREMENT NOUVEAU PATIENT");

    char nom[MAX_NOM], prenom[MAX_PRENOM], id[MAX_ID];
    int  age;

    lire_chaine("  Nom       : ", nom,    MAX_NOM);
    lire_chaine("  Prenom    : ", prenom, MAX_PRENOM);
    lire_chaine("  ID (CIN)  : ", id,     MAX_ID);
    age = lire_entier("  Age       : ", 0, 130);

    Patient *p = creer_patient(sys, nom, prenom, id, age);
    if (!p) return;

    /* En mode catastrophe les VERT ne sont pas prioritaires */
    if (sys->mode_catastrophe) {
        printf("\n  *** MODE CATASTROPHE ACTIF ***\n");
        printf("  Patient enregistre avec priorite JAUNE minimum.\n");
        p->couleur = JAUNE;
    }

    file_enqueue_priorite(&sys->file_attente, p);

    printf("\n  *** Patient enregistre avec succes ! ***\n");
    printf("  Ticket attribue : #%03d\n", p->ticket);
    printf("  Heure d'arrivee : %s\n",    p->heure_arrivee);
    printf("  -> En attente du triage infirmier.\n");
}

/* TRIAGE INFIRMIER : remplir l'état et choisir la couleur */
void menu_triage_infirmier(Systeme *sys) {
    afficher_titre("TRIAGE INFIRMIER");

    if (file_est_vide(&sys->file_attente)) {
        printf("  Aucun patient en attente.\n");
        return;
    }

    /* Affiche les patients sans couleur définie (EN_ATTENTE_INFIRMIER) */
    printf("  Patients en attente de triage :\n");
    Patient *courant = sys->file_attente.tete;
    int trouvé = 0;
    while (courant) {
        if (courant->statut == EN_ATTENTE_INFIRMIER) {
            printf("    Ticket #%03d - %s %s\n",
                   courant->ticket, courant->prenom, courant->nom);
            trouvé = 1;
        }
        courant = courant->suivant;
    }
    if (!trouvé) {
        printf("  Tous les patients ont ete triages.\n");
        return;
    }

    int ticket = lire_entier("\n  Entrez le numero de ticket : ", 1, 9999);

    /* Chercher ce patient dans la file */
    courant = sys->file_attente.tete;
    Patient *patient = NULL;
    while (courant) {
        if (courant->ticket == ticket) { patient = courant; break; }
        courant = courant->suivant;
    }

    if (!patient) {
        printf("  Ticket #%03d introuvable dans la file.\n", ticket);
        return;
    }
    if (patient->statut != EN_ATTENTE_INFIRMIER) {
        printf("  Ce patient a deja ete trie.\n");
        return;
    }

    /* Remplir l'état */
    lire_chaine("  Etat du patient (description) : ", patient->etat, MAX_ETAT);

    /* Choisir la couleur (sans utiliser le temps) */
    printf("\n  Choisir la priorite :\n");
    printf("    1. ");
    afficher_couleur_ansi(ROUGE);
    printf("ROUGE (Critique - passage immediat)");
    reset_couleur_ansi();
    printf("\n");
    printf("    2. ");
    afficher_couleur_ansi(JAUNE);
    printf("JAUNE (Urgent - peut attendre un peu)");
    reset_couleur_ansi();
    printf("\n");
    printf("    3. ");
    afficher_couleur_ansi(VERT);
    printf("VERT  (Non urgent - peut attendre plus)");
    reset_couleur_ansi();
    printf("\n");

    int choix = lire_entier("  Votre choix : ", 1, 3);

    /* Retirer le patient de la file, changer sa couleur, remettre */
    /* On retire manuellement par ticket */
    Patient *prec = NULL;
    courant = sys->file_attente.tete;
    while (courant && courant->ticket != ticket) {
        prec    = courant;
        courant = courant->suivant;
    }
    if (courant) {
        if (prec) prec->suivant = courant->suivant;
        else      sys->file_attente.tete = courant->suivant;
        if (sys->file_attente.tete == NULL) sys->file_attente.queue = NULL;
        sys->file_attente.taille--;
        courant->suivant = NULL;
    }

    patient->couleur = choix;
    patient->statut  = EN_ATTENTE_MEDECIN;

    /* Réinsérer avec la nouvelle priorité */
    file_enqueue_priorite(&sys->file_attente, patient);

    printf("\n  Triage effectue !\n");
    printf("  Patient #%03d -> ", ticket);
    afficher_couleur_ansi(patient->couleur);
    printf("%s", couleur_str(patient->couleur));
    reset_couleur_ansi();
    printf("\n");
}

/* APPELER LE PROCHAIN PATIENT (vers un médecin) */
void menu_appeler_prochain(Systeme *sys) {
    afficher_titre("APPELER PROCHAIN PATIENT");

    if (file_est_vide(&sys->file_attente)) {
        printf("  Aucun patient en file d'attente.\n");
        return;
    }

    /* Vérifier qu'un médecin est dispo */
    int idx = trouver_medecin_dispo(sys);
    if (idx == -1) {
        printf("  Aucun medecin disponible pour le moment.\n");
        afficher_medecins(sys);
        return;
    }

    /* Choisir quel médecin si plusieurs dispo */
    printf("  Medecins disponibles :\n");
    for (int i = 0; i < sys->nb_medecins; i++) {
        if (sys->medecins[i].statut == DISPONIBLE)
            printf("    [%d] Dr. %s\n", i+1, sys->medecins[i].nom);
    }
    int choix_med = lire_entier("  Choisir medecin (numero) : ", 1, sys->nb_medecins) - 1;
    if (sys->medecins[choix_med].statut != DISPONIBLE) {
        printf("  Ce medecin n'est pas disponible.\n");
        return;
    }

    Patient *p = file_dequeue(&sys->file_attente);
    if (!p) return;

    medecin_prend_patient(sys, choix_med, p);

    printf("\n");
    afficher_separateur('*', 50);
    printf("  PATIENT APPELE :\n");
    afficher_patient(p);
    printf("  -> Assigne a Dr. %s\n", sys->medecins[choix_med].nom);
    afficher_separateur('*', 50);

    /* Stocker le patient temporairement dans l'historique en consultation */
    /* On le met dans l'historique provisoirement - il sera mis à jour lors du résultat */
    hist_ajouter(&sys->historique, p);
}

/*  RÉSULTAT DE CONSULTATION */
void menu_resultat_consultation(Systeme *sys) {
    afficher_titre("RESULTAT CONSULTATION");

    /* Affiche les patients EN_CONSULTATION */
    printf("  Patients en consultation :\n");
    NoeudHist *courant = sys->historique.tete;
    int trouvé = 0;
    while (courant) {
        if (courant->patient->statut == EN_CONSULTATION) {
            printf("    Ticket #%03d - %s %s - Dr. %s\n",
                   courant->patient->ticket,
                   courant->patient->prenom,
                   courant->patient->nom,
                   courant->patient->medecin_assigne);
            trouvé = 1;
        }
        courant = courant->suivant;
    }
    if (!trouvé) {
        printf("  Aucun patient en consultation actuellement.\n");
        return;
    }

    int ticket = lire_entier("\n  Ticket du patient : ", 1, 9999);

    /* Retrouver le patient */
    Patient *p = NULL;
    courant = sys->historique.tete;
    while (courant) {
        if (courant->patient->ticket == ticket &&
            courant->patient->statut == EN_CONSULTATION) {
            p = courant->patient;
            break;
        }
        courant = courant->suivant;
    }
    if (!p) {
        printf("  Ticket introuvable parmi les consultations en cours.\n");
        return;
    }

    /* Libérer le médecin */
    int idx_med = medecin_index_par_nom(sys, p->medecin_assigne);
    if (idx_med >= 0) medecin_libere(sys, idx_med);

    printf("\n  Decision du medecin :\n");
    printf("    1. Ordonnance et sortie\n");
    printf("    2. Mise sous observation (lit)\n");
    printf("    3. Transfert vers un autre departement\n");

    int choix = lire_entier("  Votre choix : ", 1, 3);

    switch (choix) {
        case 1: {
            lire_chaine("  Ordonnance : ", p->ordonnance, MAX_ORDONNANCE);
            p->statut = SORTI_ORDONNANCE;
            sys->stat_discharges++;
            printf("  -> Patient #%03d sorti avec ordonnance.\n", ticket);
            break;
        }
        case 2: {
            if (!obs_ajouter(&sys->observation, p)) {
                printf("  Observation impossible (plus de lits).\n");
            } else {
                sys->stat_hospitalises++;
                printf("  -> Patient #%03d place en observation, lit n°%d.\n",
                       ticket, p->lit);
            }
            break;
        }
        case 3: {
            char dept[MAX_DEPT];
            printf("  Departements : Cardiologie, Radiologie, Chirurgie,\n");
            printf("                 Neurologie, Pediatrie, Orthopedie...\n");
            lire_chaine("  Departement : ", dept, MAX_DEPT);
            strncpy(p->departement_ref, dept, MAX_DEPT - 1);
            p->statut = REFERE;
            sys->stat_referes++;
            printf("  -> Patient #%03d refere a %s.\n", ticket, dept);
            break;
        }
    }
}

/* LIBÉRER UN PATIENT DE L'OBSERVATION */
void menu_liberer_observation(Systeme *sys) {
    afficher_titre("LIBERATION OBSERVATION");

    if (sys->observation.taille == 0) {
        printf("  Aucun patient sous observation.\n");
        return;
    }
    afficher_observation(&sys->observation);

    int ticket = lire_entier("\n  Ticket du patient a liberer : ", 1, 9999);
    Patient *p = obs_retirer(&sys->observation, ticket);
    if (!p) {
        printf("  Ticket #%03d introuvable en observation.\n", ticket);
        return;
    }

    printf("  Decision de sortie :\n");
    printf("    1. Retour a domicile (ordonnance)\n");
    printf("    2. Transfert departement\n");
    int choix = lire_entier("  Choix : ", 1, 2);

    if (choix == 1) {
        lire_chaine("  Ordonnance : ", p->ordonnance, MAX_ORDONNANCE);
        p->statut = SORTI_ORDONNANCE;
        sys->stat_discharges++;
        printf("  -> Patient #%03d sorti (apres observation).\n", ticket);
    } else {
        char dept[MAX_DEPT];
        lire_chaine("  Departement : ", dept, MAX_DEPT);
        strncpy(p->departement_ref, dept, MAX_DEPT - 1);
        p->statut = REFERE;
        sys->stat_referes++;
        printf("  -> Patient #%03d refere a %s.\n", ticket, dept);
    }
    /* S'assurer qu'il est dans l'historique */
    NoeudHist *courant = sys->historique.tete;
    int dans_hist = 0;
    while (courant) {
        if (courant->patient->ticket == ticket) { dans_hist = 1; break; }
        courant = courant->suivant;
    }
    if (!dans_hist) hist_ajouter(&sys->historique, p);
}

/* AFFICHAGES*/
void menu_affichages(Systeme *sys) {
    int continuer = 1;
    while (continuer) {
        afficher_titre("AFFICHAGES");
        printf("  1. File d'attente\n");
        printf("  2. Patients sous observation\n");
        printf("  3. Historique complet\n");
        printf("  4. Etat des medecins\n");
        printf("  5. Retour\n");
        int choix = lire_entier("  Choix : ", 1, 5);
        printf("\n");
        switch (choix) {
            case 1: afficher_file(&sys->file_attente); break;
            case 2: afficher_observation(&sys->observation); break;
            case 3: afficher_historique(&sys->historique); break;
            case 4: afficher_medecins(sys); break;
            case 5: continuer = 0; break;
        }
        if (choix != 5) { printf("\n"); }
    }
}

/*  STATISTIQUES */
void menu_statistiques(Systeme *sys) {
    afficher_titre("STATISTIQUES DU JOUR");

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_buf[20];
    strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", tm_info);

    afficher_separateur('=', 45);
    printf("  RAPPORT - %s\n", date_buf);
    afficher_separateur('=', 45);
    printf("  Total patients        : %d\n", sys->stat_total);
    printf("  Discharges            : %d\n", sys->stat_discharges);
    printf("  Hospitalises          : %d\n", sys->stat_hospitalises);
    printf("  Referes               : %d\n", sys->stat_referes);
    printf("  En attente (actuel)   : %d\n", sys->file_attente.taille);
    printf("  En observation        : %d\n", sys->observation.taille);
    if (sys->mode_catastrophe)
        printf("  *** MODE CATASTROPHE ACTIF (%d victimes) ***\n",
               sys->nb_catastrophe_victimes);
    afficher_separateur('-', 45);

    printf("  MEDECINS\n");
    for (int i = 0; i < sys->nb_medecins; i++) {
        printf("  Dr. %-18s : %d patients\n",
               sys->medecins[i].nom,
               sys->medecins[i].patients_traites);
    }
    afficher_separateur('=', 45);

    /* Option sauvegarde rapport */
    printf("\n  Sauvegarder le rapport ? (1=Oui 0=Non) : ");
    int rep;
    scanf("%d", &rep); vider_buffer();
    if (rep == 1) {
        char nom_fichier[100];
        snprintf(nom_fichier, sizeof(nom_fichier), "rapport_%s.txt", date_buf);
        /* Remplacer / par - dans la date pour le nom de fichier */
        for (int i = 0; nom_fichier[i]; i++)
            if (nom_fichier[i] == '/') nom_fichier[i] = '-';
        sauvegarder_rapport_journalier(sys, nom_fichier);
        sauvegarder_historique(&sys->historique, "historique.txt");
    }
}

/* MENU MÉDECINS */
void menu_medecins(Systeme *sys) {
    afficher_titre("GESTION MEDECINS");
    printf("  1. Voir l'etat des medecins\n");
    printf("  2. Marquer medecin DISPONIBLE\n");
    printf("  3. Marquer medecin NON DISPONIBLE\n");
    printf("  4. Retour\n");
    int choix = lire_entier("  Choix : ", 1, 4);

    switch (choix) {
        case 1:
            afficher_medecins(sys);
            break;
        case 2: {
            afficher_medecins(sys);
            int n = lire_entier("  Numero du medecin : ", 1, sys->nb_medecins) - 1;
            sys->medecins[n].statut = DISPONIBLE;
            printf("  Dr. %s -> DISPONIBLE\n", sys->medecins[n].nom);
            break;
        }
        case 3: {
            afficher_medecins(sys);
            int n = lire_entier("  Numero du medecin : ", 1, sys->nb_medecins) - 1;
            sys->medecins[n].statut = NON_DISPONIBLE;
            printf("  Dr. %s -> NON DISPONIBLE\n", sys->medecins[n].nom);
            break;
        }
        case 4: break;
    }
}

/* MENU CATASTROPHE */
void menu_catastrophe(Systeme *sys) {
    afficher_titre("MODE CATASTROPHE");
    printf("  Statut actuel : %s\n",
           sys->mode_catastrophe ? "\033[1;31mACTIF\033[0m" : "\033[1;32mINACTIF\033[0m");
    printf("  1. Activer le mode catastrophe\n");
    printf("  2. Desactiver le mode catastrophe\n");
    printf("  3. Retour\n");
    int choix = lire_entier("  Choix : ", 1, 3);
    if (choix == 1) activer_mode_catastrophe(sys);
    else if (choix == 2) desactiver_mode_catastrophe(sys);
}

/* MENU PRINCIPAL */
void menu_principal(Systeme *sys) {
    int quitter = 0;
    while (!quitter) {
        printf("\n");
        afficher_separateur('=', 55);
        printf("   URGENCES - SYSTEME DE GESTION\n");
        if (sys->mode_catastrophe)
            printf("   \033[1;31m*** MODE CATASTROPHE ACTIF ***\033[0m\n");
        printf("   File : %d patient(s) | Observation : %d\n",
               sys->file_attente.taille, sys->observation.taille);
        afficher_separateur('=', 55);
        printf("   1.  Enregistrer un nouveau patient\n");
        printf("   2.  Triage infirmier (etat + couleur)\n");
        printf("   3.  Appeler le prochain patient (vers medecin)\n");
        printf("   4.  Enregistrer resultat consultation\n");
        printf("   5.  Liberer patient de l'observation\n");
        printf("   6.  Affichages\n");
        printf("   7.  Statistiques & Rapport\n");
        printf("   8.  Gestion medecins\n");
        printf("   9.  Mode catastrophe\n");
        printf("   10. Quitter\n");
        afficher_separateur('-', 55);

        int choix = lire_entier("   Votre choix : ", 1, 10);
        printf("\n");

        switch (choix) {
            case 1:  menu_enregistrer_patient(sys);    break;
            case 2:  menu_triage_infirmier(sys);       break;
            case 3:  menu_appeler_prochain(sys);       break;
            case 4:  menu_resultat_consultation(sys);  break;
            case 5:  menu_liberer_observation(sys);    break;
            case 6:  menu_affichages(sys);             break;
            case 7:  menu_statistiques(sys);           break;
            case 8:  menu_medecins(sys);               break;
            case 9:  menu_catastrophe(sys);            break;
            case 10: quitter = 1;                      break;
        }
    }
}
