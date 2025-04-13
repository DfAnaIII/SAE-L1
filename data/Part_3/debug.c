#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIGNE 256
#define MAX_MOTS 50
#define MAX_REGLES 100
#define MAX_ETATS 1000

typedef char string[50];

/**
 * Structure représentant une règle du système GPS
 */
typedef struct {
    string action;
    string preconds[MAX_MOTS];
    string adds[MAX_MOTS];
    string deletes[MAX_MOTS];
    int nb_preconds;
    int nb_adds;
    int nb_deletes;
    int indice;  // Indice de la règle dans le tableau des règles
} Regle;

/**
 * Structure représentant un état du système avec mémoire
 */
typedef struct {
    string faits[MAX_MOTS];
    int nb_faits;
    int regle_appliquee;  // Indice de la règle qui a mené à cet état
    int parent;           // Indice de l'état parent
} Etat;

/**
 * Fonction pour charger les règles à partir d'un fichier
 */
int chargerRegles(const char *fichier, Regle regles[]) {
    FILE *f = fopen(fichier, "r");
    if(!f) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", fichier);
        return 0;
    }
    
    char ligne[MAX_LIGNE];
    int nb_regles = 0;
    
    while(fgets(ligne, MAX_LIGNE, f) && nb_regles < MAX_REGLES) {
        // Format attendu: action|precond1,precond2,...|add1,add2,...|del1,del2,...
        ligne[strcspn(ligne, "\n")] = 0;  // Enlever le retour à la ligne
        
        char *token = strtok(ligne, "|");
        if(!token) continue;
        
        strcpy(regles[nb_regles].action, token);
        regles[nb_regles].nb_preconds = 0;
        regles[nb_regles].nb_adds = 0;
        regles[nb_regles].nb_deletes = 0;
        
        // Préconditions
        token = strtok(NULL, "|");
        if(token) {
            char *precond = strtok(token, ",");
            while(precond && regles[nb_regles].nb_preconds < MAX_MOTS) {
                strcpy(regles[nb_regles].preconds[regles[nb_regles].nb_preconds++], precond);
                precond = strtok(NULL, ",");
            }
        }
        
        // Ajouts
        token = strtok(NULL, "|");
        if(token) {
            char *add = strtok(token, ",");
            while(add && regles[nb_regles].nb_adds < MAX_MOTS) {
                strcpy(regles[nb_regles].adds[regles[nb_regles].nb_adds++], add);
                add = strtok(NULL, ",");
            }
        }
        
        // Suppressions
        token = strtok(NULL, "|");
        if(token) {
            char *del = strtok(token, ",");
            while(del && regles[nb_regles].nb_deletes < MAX_MOTS) {
                strcpy(regles[nb_regles].deletes[regles[nb_regles].nb_deletes++], del);
                del = strtok(NULL, ",");
            }
        }
        
        nb_regles++;
    }
    
    fclose(f);
    return nb_regles;
}

/**
 * Fonction pour charger des faits à partir d'un fichier
 */
int chargerFaits(const char *fichier, string faits[]) {
    FILE *f = fopen(fichier, "r");
    if(!f) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", fichier);
        return 0;
    }
    
    char ligne[MAX_LIGNE];
    int nb_faits = 0;
    
    while(fgets(ligne, MAX_LIGNE, f) && nb_faits < MAX_MOTS) {
        ligne[strcspn(ligne, "\n")] = 0;  // Enlever le retour à la ligne
        if(strlen(ligne) > 0) {
            strcpy(faits[nb_faits++], ligne);
        }
    }
    
    fclose(f);
    return nb_faits;
}

/**
 * Vérifie si un fait existe dans un état
 */
int contientFait(Etat *etat, const char *fait) {
    for(int i = 0; i < etat->nb_faits; i++) {
        if(strcmp(etat->faits[i], fait) == 0) return 1;
    }
    return 0;
}

/**
 * Vérifie si toutes les préconditions d'une règle sont satisfaites dans un état
 */
int preconditionsSatisfaites(Regle *regle, Etat *etat) {
    for(int i = 0; i < regle->nb_preconds; i++) {
        if(!contientFait(etat, regle->preconds[i])) return 0;
    }
    return 1;
}

/**
 * Applique une règle à un état pour créer un nouvel état
 */
void appliquerRegle(Regle *regle, Etat *source, Etat *destination) {
    // Copie des faits existants
    destination->nb_faits = 0;
    for(int i = 0; i < source->nb_faits; i++) {
        int aSupprimer = 0;
        for(int j = 0; j < regle->nb_deletes; j++) {
            if(strcmp(source->faits[i], regle->deletes[j]) == 0) {
                aSupprimer = 1;
                break;
            }
        }
        if(!aSupprimer) {
            strcpy(destination->faits[destination->nb_faits++], source->faits[i]);
        }
    }
    
    // Ajout des nouveaux faits
    for(int i = 0; i < regle->nb_adds; i++) {
        if(!contientFait(destination, regle->adds[i])) {
            strcpy(destination->faits[destination->nb_faits++], regle->adds[i]);
        }
    }
    
    // Enregistrement de la règle appliquée
    destination->regle_appliquee = regle->indice;
}

/**
 * Vérifie si tous les buts sont atteints dans un état
 */
int butsAtteints(Etat *etat, string buts[], int nb_buts) {
    for(int i = 0; i < nb_buts; i++) {
        if(!contientFait(etat, buts[i])) return 0;
    }
    return 1;
}

/**
 * Affiche les faits d'un état
 */
void afficherEtat(Etat *etat) {
    printf("Faits : ");
    for(int i = 0; i < etat->nb_faits; i++) {
        printf("%s", etat->faits[i]);
        if(i < etat->nb_faits - 1) printf(", ");
    }
    printf("\n");
}

/**
 * Recherche avec backtracking pour trouver une solution avec affichage détaillé
 */
int rechercheSolutionBacktrackDebug(Etat etats[], int *nb_etats, Regle regles[], int nb_regles, 
                             string buts[], int nb_buts) {
    int possible = 1;
    int IR = 0;  // Indice à partir duquel on cherche une règle applicable
    int etat_courant = 0;
    int profondeur = 0;
    int nb_backtracks = 0;
    
    printf("\n=== DÉBUT DE LA RECHERCHE AVEC BACKTRACKING ===\n\n");
    
    while(possible && !butsAtteints(&etats[etat_courant], buts, nb_buts)) {
        int regle_trouvee = 0;
        
        printf("État actuel #%d (profondeur %d):\n", etat_courant, profondeur);
        afficherEtat(&etats[etat_courant]);
        printf("Recherche d'une règle applicable à partir de l'indice IR=%d\n", IR);
        
        // Vérification des buts
        printf("Vérification des buts: ");
        int nb_buts_atteints = 0;
        for(int i = 0; i < nb_buts; i++) {
            if(contientFait(&etats[etat_courant], buts[i])) {
                nb_buts_atteints++;
                printf("'%s' ✓ ", buts[i]);
            } else {
                printf("'%s' ✗ ", buts[i]);
            }
        }
        printf("(%d/%d atteints)\n", nb_buts_atteints, nb_buts);
        
        // Cherche une règle applicable d'indice > IR
        for(int i = IR; i < nb_regles; i++) {
            printf("  Évaluation règle #%d ('%s'): ", i, regles[i].action);
            
            if(preconditionsSatisfaites(&regles[i], &etats[etat_courant])) {
                printf("✓ (préconditions satisfaites)\n");
                
                // Applique la règle pour créer un nouvel état
                etats[*nb_etats].parent = etat_courant;
                appliquerRegle(&regles[i], &etats[etat_courant], &etats[*nb_etats]);
                
                printf("  Application de la règle '%s'\n", regles[i].action);
                printf("  Nouvel état créé #%d:\n", *nb_etats);
                afficherEtat(&etats[*nb_etats]);
                
                etat_courant = *nb_etats;
                (*nb_etats)++;
                profondeur++;
                IR = 0;  // Réinitialise IR pour le nouvel état
                printf("  IR réinitialisé à 0 pour le nouvel état\n");
                regle_trouvee = 1;
                break;
            } else {
                printf("✗ (préconditions non satisfaites)\n");
            }
        }
        
        if(!regle_trouvee) {
            printf("Aucune règle applicable trouvée pour l'état #%d\n", etat_courant);
            
            if(etat_courant > 0) {
                // Backtrack: revient à l'état précédent
                int ancien_etat = etat_courant;
                IR = etats[etat_courant].regle_appliquee + 1;
                etat_courant = etats[etat_courant].parent;
                profondeur--;
                (*nb_etats)--;  // Oublie l'état courant
                
                printf("BACKTRACK! Retour à l'état parent #%d\n", etat_courant);
                printf("IR mis à %d (règle suivante à tester)\n", IR);
                nb_backtracks++;
            } else {
                possible = 0;  // Aucune solution trouvée
                printf("Impossible de backtracker depuis l'état initial. ÉCHEC.\n");
            }
        }
        
        printf("\n");
    }
    
    printf("=== FIN DE LA RECHERCHE ===\n");
    printf("Nombre total d'états explorés: %d\n", *nb_etats);
    printf("Nombre de backtracks effectués: %d\n", nb_backtracks);
    
    if(possible) {
        printf("SUCCÈS! Tous les buts sont atteints dans l'état #%d\n", etat_courant);
    } else {
        printf("ÉCHEC! Aucune solution trouvée après exploration complète.\n");
    }
    
    return possible ? etat_courant : -1;
}

/**
 * Affiche la solution trouvée avec des détails
 */
void afficherSolutionDetaillee(Etat etats[], Regle regles[], int indice_solution) {
    if(indice_solution < 0) {
        printf("Aucune solution trouvée.\n");
        return;
    }
    
    // Reconstruit le chemin de la solution
    int chemin[MAX_ETATS];
    int longueur = 0;
    int indice = indice_solution;
    
    while(indice > 0) {
        chemin[longueur++] = indice;
        indice = etats[indice].parent;
    }
    
    printf("\n=== SOLUTION DÉTAILLÉE (%d étapes) ===\n", longueur);
    printf("État initial:\n");
    afficherEtat(&etats[0]);
    
    for(int i = longueur - 1; i >= 0; i--) {
        int etat_indice = chemin[i];
        int regle_indice = etats[etat_indice].regle_appliquee;
        
        printf("\nÉtape %d: Application de la règle #%d '%s'\n", 
               longueur - i, regle_indice, regles[regle_indice].action);
        
        printf("Préconditions: ");
        for(int j = 0; j < regles[regle_indice].nb_preconds; j++) {
            printf("%s", regles[regle_indice].preconds[j]);
            if(j < regles[regle_indice].nb_preconds - 1) printf(", ");
        }
        printf("\n");
        
        printf("Ajouts: ");
        for(int j = 0; j < regles[regle_indice].nb_adds; j++) {
            printf("%s", regles[regle_indice].adds[j]);
            if(j < regles[regle_indice].nb_adds - 1) printf(", ");
        }
        printf("\n");
        
        printf("Suppressions: ");
        for(int j = 0; j < regles[regle_indice].nb_deletes; j++) {
            printf("%s", regles[regle_indice].deletes[j]);
            if(j < regles[regle_indice].nb_deletes - 1) printf(", ");
        }
        printf("\n");
        
        printf("Nouvel état obtenu:\n");
        afficherEtat(&etats[etat_indice]);
    }
}

int main() {
    Regle regles[MAX_REGLES];
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Etat etats[MAX_ETATS];
    int nb_etats = 1;
    
    // Chargement des données
    int nb_regles = chargerRegles("assets/regles.txt", regles);
    int nb_faits = chargerFaits("assets/faits.txt", faits_initiaux);
    int nb_buts = chargerFaits("assets/buts.txt", buts);
    
    printf("=== SYSTÈME GPS AVEC BACKTRACKING (DEBUG) ===\n");
    printf("Règles chargées: %d\n", nb_regles);
    printf("Faits initiaux: %d\n", nb_faits);
    printf("Buts: %d\n", nb_buts);
    
    // Affichage des règles
    printf("\nRègles:\n");
    for(int i = 0; i < nb_regles; i++) {
        printf("Règle #%d: %s\n", i, regles[i].action);
        printf("  Préconditions (%d): ", regles[i].nb_preconds);
        for(int j = 0; j < regles[i].nb_preconds; j++) {
            printf("%s", regles[i].preconds[j]);
            if(j < regles[i].nb_preconds - 1) printf(", ");
        }
        printf("\n");
        
        printf("  Ajouts (%d): ", regles[i].nb_adds);
        for(int j = 0; j < regles[i].nb_adds; j++) {
            printf("%s", regles[i].adds[j]);
            if(j < regles[i].nb_adds - 1) printf(", ");
        }
        printf("\n");
        
        printf("  Suppressions (%d): ", regles[i].nb_deletes);
        for(int j = 0; j < regles[i].nb_deletes; j++) {
            printf("%s", regles[i].deletes[j]);
            if(j < regles[i].nb_deletes - 1) printf(", ");
        }
        printf("\n");
    }
    
    // Affichage des faits initiaux
    printf("\nFaits initiaux:\n");
    for(int i = 0; i < nb_faits; i++) {
        printf("  %s\n", faits_initiaux[i]);
    }
    
    // Affichage des buts
    printf("\nButs:\n");
    for(int i = 0; i < nb_buts; i++) {
        printf("  %s\n", buts[i]);
    }
    
    // Initialisation des indices des règles
    for(int i = 0; i < nb_regles; i++) {
        regles[i].indice = i;
    }
    
    // Initialisation de l'état initial
    etats[0].nb_faits = nb_faits;
    etats[0].parent = -1;
    etats[0].regle_appliquee = -1;
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etats[0].faits[i], faits_initiaux[i]);
    }
    
    // Recherche d'une solution avec backtracking et debug
    int solution = rechercheSolutionBacktrackDebug(etats, &nb_etats, regles, nb_regles, buts, nb_buts);
    
    // Affichage détaillé de la solution
    afficherSolutionDetaillee(etats, regles, solution);
    
    return 0;
}