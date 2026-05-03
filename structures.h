#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* CONSTANTES  */
#define MAX_NOM        50
#define MAX_PRENOM     50
#define MAX_ID         20
#define MAX_ETAT       200
#define MAX_ORDONNANCE 300
#define MAX_DEPT       50
#define MAX_MEDECINS   10

/* Couleurs de triage */
#define ROUGE 1   /* Critique   - passage immediat     */
#define JAUNE 2   /* Urgent     - peut attendre un peu */
#define VERT  3   /* Non urgent - peut attendre plus   */

/* Statut du patient */
#define EN_ATTENTE_INFIRMIER  0
#define EN_ATTENTE_MEDECIN    1
#define EN_CONSULTATION       2
#define SOUS_OBSERVATION      3
#define SORTI_ORDONNANCE      4
#define HOSPITALISE           5
#define REFERE                6
#define CATASTROPHE_VICTIME   7

/* Statut du medecin */
#define DISPONIBLE     0
#define NON_DISPONIBLE 1

/* STRUCTURE PATIENT*/
typedef struct t_patient {
    int    ticket;
    char   id[MAX_ID];
    char   nom[MAX_NOM];
    char   prenom[MAX_PRENOM];
    int    age;
    char   etat[MAX_ETAT];
    int    couleur;              
    int    statut;
    char   heure_arrivee[20];
    char   medecin_assigne[MAX_NOM];
    char   ordonnance[MAX_ORDONNANCE];
    char   departement_ref[MAX_DEPT];
    int    lit;
    struct t_patient *suivant;    
} Patient;

/*  FILE D'ATTENTE */
typedef struct {
    Patient *tete;   
    Patient *queue;   
    int      taille;
} FileAttente;

/* LISTE OBSERVATION  */
typedef struct t_noeud_obs {
    Patient            *patient;
    struct t_noeud_obs *suivant;
} NoeudObs;

typedef struct {
    NoeudObs *tete;
    int       taille;
    int       lits_occupes[20];
} ListeObservation;

/* HISTORIQUE */
typedef struct t_noeud_hist {
    Patient             *patient;
    struct t_noeud_hist *suivant;
} NoeudHist;

typedef struct {
    NoeudHist *tete;
    int        taille;
} Historique;

/*  MEDECIN */
typedef struct {
    char nom[MAX_NOM];
    int  statut;
    int  patients_traites;
} Medecin;

/*  SYSTEME GLOBAL */
typedef struct {
    FileAttente      file_attente;
    ListeObservation observation;
    Historique       historique;
    Medecin          medecins[MAX_MEDECINS];
    int              nb_medecins;
    int              compteur_ticket;
    int              mode_catastrophe;
    int              nb_catastrophe_victimes;
    int              stat_total;
    int              stat_discharges;
    int              stat_hospitalises;
    int              stat_referes;
} Systeme;

/* PROTOTYPES - patient.c */
Patient*    creer_patient(Systeme *sys, const char *nom, const char *prenom,   const char *id, int age);
void        afficher_patient(const Patient *p);
const char* couleur_str(int c);
const char* statut_str(int s);
void        get_heure_actuelle(char *buf, int taille);

/* PROTOTYPES - file.c */
void     file_init(FileAttente *f);
void     file_enqueue_priorite(FileAttente *f, Patient *p);
Patient* file_dequeue(FileAttente *f);
int      file_est_vide(const FileAttente *f);
void     afficher_file(const FileAttente *f);

/* PROTOTYPES - observation.c */
void     obs_init(ListeObservation *lo);
int      obs_ajouter(ListeObservation *lo, Patient *p);
Patient* obs_retirer(ListeObservation *lo, int ticket);
void     afficher_observation(const ListeObservation *lo);

/* PROTOTYPES - medecin.c */
void init_medecins(Systeme *sys);
void afficher_medecins(const Systeme *sys);
int  trouver_medecin_dispo(const Systeme *sys);
void medecin_prend_patient(Systeme *sys, int idx, Patient *p);
void medecin_libere(Systeme *sys, int idx);
int  medecin_index_par_nom(const Systeme *sys, const char *nom);

/*PROTOTYPES - historique.c*/
void hist_init(Historique *h);
void hist_ajouter(Historique *h, Patient *p);
void afficher_historique(const Historique *h);

/*  PROTOTYPES - fichiers.c*/
void sauvegarder_historique(const Historique *h, const char *fichier);
void sauvegarder_rapport_journalier(const Systeme *sys, const char *fichier);
void charger_historique(Historique *h, const char *fichier);

/*  PROTOTYPES - catastrophe.c */
void activer_mode_catastrophe(Systeme *sys);
void desactiver_mode_catastrophe(Systeme *sys);

/* PROTOTYPES - menu.c */
void menu_principal(Systeme *sys);
void menu_enregistrer_patient(Systeme *sys);
void menu_triage_infirmier(Systeme *sys);
void menu_appeler_prochain(Systeme *sys);
void menu_resultat_consultation(Systeme *sys);
void menu_liberer_observation(Systeme *sys);
void menu_affichages(Systeme *sys);
void menu_statistiques(Systeme *sys);
void menu_medecins(Systeme *sys);
void menu_catastrophe(Systeme *sys);

/* PROTOTYPES - utils.c */
void vider_buffer(void);
int  lire_entier(const char *msg, int min, int max);
void lire_chaine(const char *msg, char *dest, int taille);
void afficher_separateur(char c, int n);
void afficher_titre(const char *titre);
void afficher_couleur_ansi(int couleur);
void reset_couleur_ansi(void);

#endif /* STRUCTURES_H */
