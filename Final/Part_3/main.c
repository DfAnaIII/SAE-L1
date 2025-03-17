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
 * Recherche avec backtracking pour trouver une solution
 */
int rechercheSolutionBacktrack(Etat etats[], int *nb_etats, Regle regles[], int nb_regles, 
                             string buts[], int nb_buts) {
    int possible = 1;
    int IR = 0;  // Indice à partir duquel on cherche une règle applicable
    int etat_courant = 0;
    
    while(possible && !butsAtteints(&etats[etat_courant], buts, nb_buts)) {
        int regle_trouvee = 0;
        
        // Cherche une règle applicable d'indice > IR
        for(int i = IR; i < nb_regles; i++) {
            if(preconditionsSatisfaites(&regles[i], &etats[etat_courant])) {
                // Applique la règle pour créer un nouvel état
                etats[*nb_etats].parent = etat_courant;
                appliquerRegle(&regles[i], &etats[etat_courant], &etats[*nb_etats]);
                
                etat_courant = *nb_etats;
                (*nb_etats)++;
                IR = 0;  // Réinitialise IR pour le nouvel état
                regle_trouvee = 1;
                break;
            }
        }
        
        if(!regle_trouvee) {
            if(etat_courant > 0) {
                // Backtrack: revient à l'état précédent
                IR = etats[etat_courant].regle_appliquee + 1;
                etat_courant = etats[etat_courant].parent;
                (*nb_etats)--;  // Oublie l'état courant
            } else {
                possible = 0;  // Aucune solution trouvée
            }
        }
    }
    
    return possible ? etat_courant : -1;
}

/**
 * Affiche la solution trouvée
 */
void afficherSolution(Etat etats[], Regle regles[], int indice_solution) {
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
    
    printf("\nSolution trouvée (%d étapes):\n", longueur);
    for(int i = longueur - 1; i >= 0; i--) {
        int regle = etats[chemin[i]].regle_appliquee;
        printf("- %s\n", regles[regle].action);
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
    
    // Recherche d'une solution avec backtracking
    int solution = rechercheSolutionBacktrack(etats, &nb_etats, regles, nb_regles, buts, nb_buts);
    
    // Affichage de la solution
    afficherSolution(etats, regles, solution);
    
    printf("\nStatistiques:\n");
    printf("- Nombre d'états explorés: %d\n", nb_etats);
    
    return 0;
}