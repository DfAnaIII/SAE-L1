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
} Regle;

/**
 * Structure représentant un état du système
 */
typedef struct {
    string faits[MAX_MOTS];
    int nb_faits;
    string action_precedente;
    int parent;
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
        // Vérifie si le fait doit être supprimé
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
    
    // Enregistrement de l'action effectuée
    strcpy(destination->action_precedente, regle->action);
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
 * Recherche en largeur d'abord pour trouver une solution
 */
int rechercheSolution(Etat etats[], int *nb_etats, Regle regles[], int nb_regles, 
                     string buts[], int nb_buts) {
    int indice_courant = 0;
    
    while(indice_courant < *nb_etats) {
        // Vérifie si l'état courant atteint les buts
        if(butsAtteints(&etats[indice_courant], buts, nb_buts)) {
            return indice_courant;
        }
        
        // Essaie d'appliquer chaque règle
        for(int i = 0; i < nb_regles; i++) {
            if(preconditionsSatisfaites(&regles[i], &etats[indice_courant])) {
                // Crée un nouvel état en appliquant la règle
                etats[*nb_etats].parent = indice_courant;
                appliquerRegle(&regles[i], &etats[indice_courant], &etats[*nb_etats]);
                (*nb_etats)++;
            }
        }
        indice_courant++;
    }
    return -1; // Aucune solution trouvée
}

/**
 * Affiche la solution trouvée
 */
void afficherSolution(Etat etats[], int indice_solution) {
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
        printf("- %s\n", etats[chemin[i]].action_precedente);
    }
}

int main() {
    Regle regles[MAX_REGLES];
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Etat etats[MAX_ETATS];
    int nb_etats = 1;  // L'état initial est déjà compté
    
    // Chargement des données (réutilisation du code de la Partie 1)
    int nb_regles = chargerRegles("assets/regles.txt", regles);
    int nb_faits = chargerFaits("assets/faits.txt", faits_initiaux);
    int nb_buts = chargerFaits("assets/buts.txt", buts);
    
    // Initialisation de l'état initial
    etats[0].nb_faits = nb_faits;
    etats[0].parent = -1;
    strcpy(etats[0].action_precedente, "Etat initial");
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etats[0].faits[i], faits_initiaux[i]);
    }
    
    // Recherche d'une solution
    int solution = rechercheSolution(etats, &nb_etats, regles, nb_regles, buts, nb_buts);
    
    // Affichage de la solution
    afficherSolution(etats, solution);
    
    printf("\nStatistiques:\n");
    printf("- Nombre d'états explorés: %d\n", nb_etats);
    
    return 0;
}
