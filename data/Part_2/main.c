#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------
#define MAX_LEN       256   // Longueur maximale d'une ligne
#define MAX_FACTS     20    // Nombre maximal de faits par état
#define MAX_ACTIONS   50    // Nombre maximal d'actions
#define MAX_LINE_PARTS 20   // Nombre maximal d'éléments par ligne
#define DEBUG         1     // Mode debug (1 = activé, 0 = désactivé)

// ---------------------------------------------------------------------
// Structures de données
// ---------------------------------------------------------------------

/**
 * @brief Représente un état avec des faits
 */
typedef struct {
    int factCount;              ///< Nombre de faits actuels
    char facts[MAX_FACTS][MAX_LEN]; ///< Liste des faits
} State;

// L'objectif (finish) est un ensemble de faits (même structure que State)
typedef State Goal;

/**
 * @brief Représente une action avec ses préconditions et effets
 */
typedef struct {
    char name[MAX_LEN];         ///< Nom de l'action
    State preconds;             ///< Préconditions nécessaires
    State addList;              ///< Faits à ajouter
    State delList;              ///< Faits à supprimer
} Action;

// ---------------------------------------------------------------------
// Fonctions de parsing
// ---------------------------------------------------------------------

/**
 * @brief Découpe une ligne en éléments séparés par des virgules
 * @param source Ligne à parser
 * @param target Tableau de sortie pour stocker les éléments
 * @return Nombre d'éléments extraits
 */
int parseLine(const char* source, char target[][MAX_LEN]) {
    int count = 0;
    char buffer[MAX_LEN];
    strcpy(buffer, source);
    
    // Trouver le premier ':' pour ignorer le préfixe
    char* start = strchr(buffer, ':');
    if(!start) return 0;
    start++; // Passer le ':'

    // Découper les éléments
    char* token = strtok(start, ",");
    while(token && count < MAX_LINE_PARTS) {
        strcpy(target[count], token);
        count++;
        token = strtok(NULL, ",");
    }
    return count;
}

/**
 * @brief Charge un fichier de problème dans les structures de données
 * @param filename Nom du fichier à charger
 * @param initial État initial à remplir
 * @param goal But à remplir
 * @param actions Tableau d'actions à remplir
 * @param actionCount Compteur d'actions
 * @return true si chargement réussi, false sinon
 */
bool loadProblemFile(const char* filename, State* initial, Goal* goal, 
                    Action* actions, int* actionCount) {
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s\n", filename);
        return false;
    }

    char line[MAX_LEN];
    Action* currentAction = NULL;
    *actionCount = 0;

    while(fgets(line, MAX_LEN, fp)) {
        // Nettoyer la ligne
        line[strcspn(line, "\r\n")] = '\0';
        
        // Gestion des sections d'action
        if(strncmp(line, "****", 4) == 0) {
            if(*actionCount < MAX_ACTIONS) {
                currentAction = &actions[(*actionCount)++];
                currentAction->preconds.factCount = 0;
                currentAction->addList.factCount = 0;
                currentAction->delList.factCount = 0;
            }
            continue;
        }

        // Découper la ligne
        char parts[MAX_LINE_PARTS][MAX_LEN];
        int partCount = parseLine(line, parts);
        if(partCount == 0) continue;

        // Remplir les structures
        if(strncmp(line, "start:", 6) == 0) {
            initial->factCount = partCount;
            for(int i = 0; i < partCount; i++)
                strcpy(initial->facts[i], parts[i]);
        }
        else if(strncmp(line, "finish:", 7) == 0) {
            goal->factCount = partCount;
            for(int i = 0; i < partCount; i++)
                strcpy(goal->facts[i], parts[i]);
        }
        else if(currentAction) {
            if(strncmp(line, "action:", 7) == 0) {
                strcpy(currentAction->name, parts[0]);
            }
            else if(strncmp(line, "preconds:", 9) == 0) {
                currentAction->preconds.factCount = partCount;
                for(int i = 0; i < partCount; i++)
                    strcpy(currentAction->preconds.facts[i], parts[i]);
            }
            else if(strncmp(line, "add:", 4) == 0) {
                currentAction->addList.factCount = partCount;
                for(int i = 0; i < partCount; i++)
                    strcpy(currentAction->addList.facts[i], parts[i]);
            }
            else if(strncmp(line, "delete:", 7) == 0) {
                currentAction->delList.factCount = partCount;
                for(int i = 0; i < partCount; i++)
                    strcpy(currentAction->delList.facts[i], parts[i]);
            }
        }
    }

    fclose(fp);
    return true;
}

// ---------------------------------------------------------------------
// Fonctions utilitaires
// ---------------------------------------------------------------------

/**
 * @brief Affiche l'état actuel (ensemble de faits)
 * @param state État à afficher
 * @param label Étiquette pour l'affichage
 */
void printState(const State* state, const char* label) {
    printf("%s (%d faits):\n", label, state->factCount);
    for(int i = 0; i < state->factCount; i++) {
        printf("  - %s\n", state->facts[i]);
    }
}

/**
 * @brief Vérifie si un fait existe dans un état
 * @param state État dans lequel chercher
 * @param fact Fait à chercher
 * @return true si le fait existe, false sinon
 */
bool hasFact(const State* state, const char* fact) {
    for(int i = 0; i < state->factCount; i++) {
        if(strcmp(state->facts[i], fact) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Ajoute un fait à un état s'il n'existe pas déjà
 * @param state État à modifier
 * @param fact Fait à ajouter
 * @return true si le fait a été ajouté, false s'il existait déjà ou si l'état est plein
 */
bool addFact(State* state, const char* fact) {
    if(hasFact(state, fact)) return false;
    
    if(state->factCount < MAX_FACTS) {
        strcpy(state->facts[state->factCount], fact);
        state->factCount++;
        return true;
    }
    return false;
}

/**
 * @brief Supprime un fait d'un état
 * @param state État à modifier
 * @param fact Fait à supprimer
 * @return true si le fait a été supprimé, false s'il n'existait pas
 */
bool removeFact(State* state, const char* fact) {
    for(int i = 0; i < state->factCount; i++) {
        if(strcmp(state->facts[i], fact) == 0) {
            // Remplacer par le dernier élément et réduire le compteur
            strcpy(state->facts[i], state->facts[state->factCount - 1]);
            state->factCount--;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------
// Fonctions du moteur de raisonnement
// ---------------------------------------------------------------------

/**
 * @brief Vérifie si une règle est applicable sur un état donné
 * @param action Règle/Action à vérifier
 * @param currentState État courant
 * @return true si toutes les préconditions sont satisfaites, false sinon
 */
bool isActionApplicable(const Action* action, const State* currentState) {
    // Vérifier que toutes les préconditions sont dans l'état courant
    for(int i = 0; i < action->preconds.factCount; i++) {
        if(!hasFact(currentState, action->preconds.facts[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Applique une règle sur un état donné
 * @param action Règle/Action à appliquer
 * @param currentState État à modifier
 */
void applyAction(const Action* action, State* currentState) {
    // Supprimer les faits de la liste delete
    for(int i = 0; i < action->delList.factCount; i++) {
        removeFact(currentState, action->delList.facts[i]);
    }
    
    // Ajouter les faits de la liste add
    for(int i = 0; i < action->addList.factCount; i++) {
        addFact(currentState, action->addList.facts[i]);
    }
}

/**
 * @brief Vérifie si l'état courant contient tous les faits du but
 * @param currentState État courant
 * @param goal But à atteindre
 * @return true si le but est atteint, false sinon
 */
bool isGoalReached(const State* currentState, const Goal* goal) {
    for(int i = 0; i < goal->factCount; i++) {
        if(!hasFact(currentState, goal->facts[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Trouve une règle applicable et la retourne
 * @param actions Tableau des actions disponibles
 * @param actionCount Nombre d'actions
 * @param currentState État courant
 * @param indexOut [out] Index de l'action trouvée
 * @return true si une action applicable a été trouvée, false sinon
 */
bool findApplicableAction(const Action* actions, int actionCount, 
                         const State* currentState, int* indexOut) {
    for(int i = 0; i < actionCount; i++) {
        if(isActionApplicable(&actions[i], currentState)) {
            *indexOut = i;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------
// Fonction principale
// ---------------------------------------------------------------------
int main() {
    State currentState = {0};  // État courant
    Goal goal = {0};           // But à atteindre
    Action actions[MAX_ACTIONS]; // Actions disponibles
    int actionCount = 0;       // Nombre d'actions chargées
    int steps = 0;             // Compteur d'étapes

    // Charger le problème depuis le fichier
    if(!loadProblemFile("assets/monkey.txt", &currentState, &goal, actions, &actionCount)) {
        return EXIT_FAILURE;
    }

    printf("=== MOTEUR DE RAISONNEMENT EN CHAÎNAGE AVANT (DEBUG=%d) ===\n\n", DEBUG);
    
    // Afficher l'état initial et le but
    printState(&currentState, "État initial");
    printf("\n");
    printState(&goal, "But à atteindre");
    
    printf("\n=== RÉSOLUTION ===\n");
    
    // Boucle principale: tant que le but n'est pas atteint
    while(!isGoalReached(&currentState, &goal)) {
        int actionIndex;
        
        // Trouver une règle applicable
        if(!findApplicableAction(actions, actionCount, &currentState, &actionIndex)) {
            printf("Échec: Aucune action applicable trouvée!\n");
            return EXIT_FAILURE;
        }
        
        steps++;
        printf("\n--- Étape %d ---\n", steps);
        
        if(DEBUG) {
            printf("Action choisie: %s\n", actions[actionIndex].name);
            printf("Préconditions (%d):\n", actions[actionIndex].preconds.factCount);
            for(int i = 0; i < actions[actionIndex].preconds.factCount; i++) {
                printf("  - %s\n", actions[actionIndex].preconds.facts[i]);
            }
        }
        
        // Appliquer la règle
        printf("Application de l'action: %s\n", actions[actionIndex].name);
        
        if(DEBUG) {
            printf("Faits à ajouter (%d):\n", actions[actionIndex].addList.factCount);
            for(int i = 0; i < actions[actionIndex].addList.factCount; i++) {
                printf("  + %s\n", actions[actionIndex].addList.facts[i]);
            }
            
            printf("Faits à supprimer (%d):\n", actions[actionIndex].delList.factCount);
            for(int i = 0; i < actions[actionIndex].delList.factCount; i++) {
                printf("  - %s\n", actions[actionIndex].delList.facts[i]);
            }
        }
        
        // Appliquer l'action sur l'état courant
        applyAction(&actions[actionIndex], &currentState);
        
        // Afficher l'état courant après application
        if(DEBUG) {
            printf("Nouvel état après application:\n");
            printState(&currentState, "État courant");
        }
    }
    
    printf("\n=== RÉSULTAT ===\n");
    printf("But atteint en %d étapes!\n", steps);
    printState(&currentState, "État final");
    
    return EXIT_SUCCESS;
}

