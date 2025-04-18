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
// Fonction principale
// ---------------------------------------------------------------------
int main() {
    State initial = {0};
    Goal goal = {0};
    Action actions[MAX_ACTIONS];
    int actionCount = 0;

    if(!loadProblemFile("assets/monkey.txt", &initial, &goal, actions, &actionCount)) {
        return EXIT_FAILURE;
    }

    // Affichage des données chargées
    printf("=== État initial ===\n");
    for(int i = 0; i < initial.factCount; i++)
        printf("- %s\n", initial.facts[i]);

    printf("\n=== But ===\n");
    for(int i = 0; i < goal.factCount; i++)
        printf("- %s\n", goal.facts[i]);

    printf("\n=== Actions (%d) ===\n", actionCount);
    for(int i = 0; i < actionCount; i++) {
        printf("Action %d: %s\n", i+1, actions[i].name);
        printf("Pre: %d | Add: %d | Del: %d\n", 
              actions[i].preconds.factCount,
              actions[i].addList.factCount,
              actions[i].delList.factCount);
    }

    return EXIT_SUCCESS;
}