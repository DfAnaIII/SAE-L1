#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

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
/**
 * @brief Obtient le temps actuel en millisecondes
 */
double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

/**
 * @brief Affiche un état avec son libellé
 */
void debugPrintState(const char* label, const State* state) {
    printf("[DEBUG] %s (%d faits):\n", label, state->factCount);
    for(int i = 0; i < state->factCount; i++)
        printf("  - %s\n", state->facts[i]);
}

int main() {
    double startTime = getCurrentTime();
    
    State initial = {0};
    Goal goal = {0};
    Action actions[MAX_ACTIONS];
    int actionCount = 0;

    double fileOpenTime = getCurrentTime();
    printf("[DEBUG] Début du chargement du fichier...\n");
    bool loadStatus = loadProblemFile("monkeys.txt", &initial, &goal, actions, &actionCount);
    printf("[DEBUG] Chargement terminé en %.2f ms\n", getCurrentTime() - fileOpenTime);

    if(!loadStatus) {
        fprintf(stderr, "[ERREUR] Échec du chargement\n");
        return EXIT_FAILURE;
    }

    debugPrintState("État initial", &initial);
    debugPrintState("But final", &goal);

    printf("\n[DEBUG] Analyse des actions :\n");
    for(int i = 0; i < actionCount; i++) {
        printf("Action #%d: %s\n", i+1, actions[i].name);
        debugPrintState("  Préconditions", &actions[i].preconds);
        debugPrintState("  Ajouts", &actions[i].addList);
        debugPrintState("  Suppressions", &actions[i].delList);
    }

    printf("\n[DEBUG] Temps total d'exécution : %.2f ms\n", 
          getCurrentTime() - startTime);
    
    return EXIT_SUCCESS;
}