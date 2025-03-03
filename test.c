#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// Configuration / limites
// ---------------------------------------------------------------------
#define MAX_LEN      256   // Taille max d'une ligne
#define MAX_FACTS    20    // Nombre max de "faits" qu'on stocke dans un état
#define MAX_ACTIONS  50    // Nombre max d'actions
#define MAX_STATES   1000  // Nombre max d'états visités lors du BFS

// ---------------------------------------------------------------------
// Structures de base
// ---------------------------------------------------------------------

// Un état est un ensemble de faits
typedef struct {
    int factCount;
    char facts[MAX_FACTS][MAX_LEN];
} State;

// L'objectif (finish) est un ensemble de faits (même structure que State)
typedef State Goal;

// Chaque action a un nom, des préconditions, une liste de faits ajoutés, une liste de faits supprimés
typedef struct {
    char name[MAX_LEN];
    State preconds;
    State addList;
    State delList;
} Action;

// Pour la BFS, on utilise un tableau "visited" : on stocke des "noeuds"
// (un état + un lien vers le parent + l'action qui y mène)
typedef struct {
    State state;            
    int parent;             // index du noeud parent dans visited[]
    int actionFromParent;   // index de l'action utilisée pour arriver ici
} Node;

// ---------------------------------------------------------------------
// Variables globales (pour simplifier l'implémentation BFS)
// ---------------------------------------------------------------------
Node visited[MAX_STATES];  // Stocke tous les états "visités"
int visitedCount = 0;      // Nombre d'états différents visités
int queueArray[MAX_STATES];// File d'indices (vers visited[])
int front = 0;             // Indice de lecture de la file
int rear = 0;              // Indice d'écriture de la file

// ---------------------------------------------------------------------
// 1) Affichage ASCII de présentation
// ---------------------------------------------------------------------
void afficherEnteteASCII() {
    printf("+----------------------------------------------+\n");
    printf("|                                              |\n");
    printf("|      Projet GPS (General Problem Solver)     |\n");
    printf("|                                              |\n");
    printf("|   Fait par Florian SILVA et Ana D'Erfurth    |\n");
    printf("|                                              |\n");
    printf("+----------------------------------------------+\n\n");
}

// ---------------------------------------------------------------------
// 2) Menu principal
// ---------------------------------------------------------------------
int afficherMenu() {
    printf("+---------------------------------------------------+\n");
    printf("|                                                   |\n");
    printf("|                 Menu principal :                  |\n");
    printf("|                                                   |\n");
    printf("|  1) Utiliser le fichier par defaut (monkeys.txt)  |\n");
    printf("|  2) Indiquer un fichier personnalise              |\n");
    printf("|  3) Creer un nouveau fichier                      |\n");
    printf("|  0) Quitter                                       |\n");
    printf("|                                                   |\n");
    printf("+---------------------------------------------------+\n");
    printf("Votre choix : ");
    int choix;
    scanf("%d", &choix);
    return choix;
}

// ---------------------------------------------------------------------
// Fonctions utilitaires pour la gestion des faits, de l'etat, etc.
// ---------------------------------------------------------------------

// Trim : retire les espaces en debut et en fin de chaine
void trim(char* str) {
    // Enlever espaces en debut
    while(*str == ' ' || *str == '\t') {
        memmove(str, str+1, strlen(str));
    }
    // Enlever espaces en fin
    char* end = str + strlen(str) - 1;
    while(end > str && (*end == ' ' || *end == '\t')) {
        *end = '\0';
        end--;
    }
}

// Decoupe une ligne en morceaux separes par des virgules, stocke dans un State
void splitFacts(const char* line, State* state) {
    state->factCount = 0;
    char buffer[MAX_LEN];
    strcpy(buffer, line);

    char* token = strtok(buffer, ",");
    while(token != NULL && state->factCount < MAX_FACTS) {
        trim(token);
        if(strlen(token) > 0) {
            strcpy(state->facts[state->factCount], token);
            state->factCount++;
        }
        token = strtok(NULL, ",");
    }
}

// Verifie que subset est inclus dans st : tous les faits de subset doivent etre dans st
int stateContainsAll(const State* st, const State* subset) {
    for(int i = 0; i < subset->factCount; i++) {
        int found = 0;
        for(int j = 0; j < st->factCount; j++) {
            if(strcmp(st->facts[j], subset->facts[i]) == 0) {
                found = 1;
                break;
            }
        }
        if(!found) return 0; 
    }
    return 1;
}

// Test si on peut appliquer l'action : toutes ses preconditions sont dans l'etat
int canApply(const State* st, const Action* action) {
    return stateContainsAll(st, &action->preconds);
}

// Applique l'action : on supprime delList, puis on ajoute addList (sans doublons)
void applyAction(const State* st, const Action* action, State* newState) {
    // Copie de l'etat actuel
    *newState = *st;

    // 1) Supprimer
    for(int d = 0; d < action->delList.factCount; d++) {
        for(int i = 0; i < newState->factCount; i++) {
            if(strcmp(newState->facts[i], action->delList.facts[d]) == 0) {
                // On supprime le fait i en decalant
                for(int k = i; k < newState->factCount - 1; k++) {
                    strcpy(newState->facts[k], newState->facts[k+1]);
                }
                newState->factCount--;
                i--; // pour re-verifier la meme position
            }
        }
    }

    // 2) Ajouter
    for(int a = 0; a < action->addList.factCount; a++) {
        int found = 0;
        for(int i = 0; i < newState->factCount; i++) {
            if(strcmp(newState->facts[i], action->addList.facts[a]) == 0) {
                found = 1;
                break;
            }
        }
        if(!found && newState->factCount < MAX_FACTS) {
            strcpy(newState->facts[newState->factCount], action->addList.facts[a]);
            newState->factCount++;
        }
    }
}

// Compare deux etats pour voir s'ils sont identiques (meme ensemble de faits)
bool sameState(const State* a, const State* b) {
    if(a->factCount != b->factCount) return false;
    // Verifier que chaque fait de a est dans b
    for(int i = 0; i < a->factCount; i++) {
        int found = 0;
        for(int j = 0; j < b->factCount; j++) {
            if(strcmp(a->facts[i], b->facts[j]) == 0) {
                found = 1;
                break;
            }
        }
        if(!found) return false;
    }
    return true;
}

// Test si l'etat satisfait l'objectif (finish)
int isGoalReached(const State* st, const Goal* goal) {
    return stateContainsAll(st, goal);
}

// ---------------------------------------------------------------------
// 3) Parsing du fichier
// ---------------------------------------------------------------------
// Lecture en 1 passe : start / finish / **** / action:... / preconds:... / add:... / delete:...
int parseFile(const char* filename, State* initial, Goal* goal, Action* actions, int* actionCount) {
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char line[MAX_LEN];
    int readingActionIndex = -1; 
    *actionCount = 0;

    // Vider initial / goal
    initial->factCount = 0;
    goal->factCount = 0;

    while(fgets(line, sizeof(line), fp)) {
        // enlever \n
        line[strcspn(line, "\r\n")] = '\0';

        if(strncmp(line, "****", 4) == 0) {
            readingActionIndex++;
            (*actionCount)++;
            continue;
        }

        if(readingActionIndex < 0) {
            // On n'est pas dans un bloc action
            if(strncmp(line, "start:", 6) == 0) {
                splitFacts(line + 6, initial);
            }
            else if(strncmp(line, "finish:", 7) == 0) {
                splitFacts(line + 7, goal);
            }
        } else {
            // On est dans un bloc action
            int idx = readingActionIndex;
            if(strncmp(line, "action:", 7) == 0) {
                strcpy(actions[idx].name, line + 7);
            }
            else if(strncmp(line, "preconds:", 9) == 0) {
                splitFacts(line + 9, &actions[idx].preconds);
            }
            else if(strncmp(line, "add:", 4) == 0) {
                splitFacts(line + 4, &actions[idx].addList);
            }
            else if(strncmp(line, "delete:", 7) == 0) {
                splitFacts(line + 7, &actions[idx].delList);
            }
        }
    }

    fclose(fp);
    return 1;
}

// ---------------------------------------------------------------------
// 4) BFS : on cherche un plan depuis 'start' jusqu'au 'goal'
// ---------------------------------------------------------------------
int bfs(const State* start, const Goal* goal, Action* actions, int actionCount) {
    // Reinit
    visitedCount = 0;
    front = 0;
    rear = 0;

    // L'etat initial -> visited[0]
    visited[0].state = *start;
    visited[0].parent = -1;
    visited[0].actionFromParent = -1;
    visitedCount = 1;

    // On push 0 dans la file
    queueArray[rear++] = 0;

    // Parcours
    while(front < rear) {
        int currentIndex = queueArray[front++];
        Node currentNode = visited[currentIndex];

        // Test objectif
        if(isGoalReached(&currentNode.state, goal)) {
            return currentIndex; // On a trouve
        }

        // Sinon, on tente toutes les actions
        for(int a = 0; a < actionCount; a++) {
            if(canApply(&currentNode.state, &actions[a])) {
                // generer un nouvel etat
                State newState;
                applyAction(&currentNode.state, &actions[a], &newState);

                // verif si deja vu
                bool dejaVu = false;
                for(int i = 0; i < visitedCount; i++) {
                    if(sameState(&newState, &visited[i].state)) {
                        dejaVu = true;
                        break;
                    }
                }
                if(!dejaVu && visitedCount < MAX_STATES) {
                    visited[visitedCount].state = newState;
                    visited[visitedCount].parent = currentIndex;
                    visited[visitedCount].actionFromParent = a;
                    queueArray[rear++] = visitedCount;
                    visitedCount++;
                }
            }
        }
    }

    // Pas trouve
    return -1;
}

// ---------------------------------------------------------------------
// 5) Reconstruction du plan (liste d'actions) a partir de l'index solution
// ---------------------------------------------------------------------
void reconstructPlan(int solutionIndex, Action* actions) {
    int plan[MAX_STATES];
    int length = 0;

    int current = solutionIndex;
    while(current != -1) {
        plan[length] = visited[current].actionFromParent;
        length++;
        current = visited[current].parent;
    }
    // plan[length-1] doit etre -1 (et correspond a l'etat initial)

    // Affiche en sens direct
    printf("\n=== PLAN TROUVE ===\n");
    int step = 1;
    // Saute le tout dernier -1
    for(int i = length - 2; i >= 0; i--) {
        int actionIndex = plan[i];
        printf("Etape %d: %s\n", step, actions[actionIndex].name);
        step++;
    }
    printf("=== FIN DU PLAN ===\n\n");
}

// ---------------------------------------------------------------------
// 6) Fonctions pour creation d'un fichier via l'utilisateur
// ---------------------------------------------------------------------

// Pose une question et recupere la reponse dans 'buffer'
void demanderTexte(const char* question, char* buffer, int bufferSize) {
    printf("%s", question);
    fgets(buffer, bufferSize, stdin);
    // enlever \n
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

// Cree un fichier en format correct
// start:...
// finish:...
// puis un certain nombre d'actions, chacune separee par ****
int creerFichier(char* nomFichier) {
    // On demande a l'utilisateur un nom de fichier
    demanderTexte("Entrez le nom du fichier a creer (ex: monFichier.txt) : ", nomFichier, MAX_LEN);
    
    FILE* fp = fopen(nomFichier, "w");
    if(!fp) {
        printf("Impossible de creer le fichier '%s'.\n", nomFichier);
        return 0;
    }

    // 1) Conditions initiales
    char buf[MAX_LEN];
    demanderTexte("Entrez les conditions initiales (separees par des virgules):\n> ", buf, MAX_LEN);
    fprintf(fp, "start:%s\n", buf);

    // 2) Conditions d'arrivee
    demanderTexte("Entrez les conditions d'arrivee (finish), separees par des virgules:\n> ", buf, MAX_LEN);
    fprintf(fp, "finish:%s\n", buf);

    // 3) Combien d'actions ?
    int nbActions = 0;
    printf("Combien d'actions voulez-vous saisir ? ");
    scanf("%d", &nbActions);
    getchar(); // flush \n

    for(int i=0; i<nbActions; i++){
        fprintf(fp, "****\n");
        // Action (nom)
        demanderTexte("\nNom de l'action : ", buf, MAX_LEN);
        fprintf(fp, "action:%s\n", buf);

        // preconds
        demanderTexte("Preconditions (separees par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "preconds:%s\n", buf);

        // add
        demanderTexte("Faits ajoutes (separes par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "add:%s\n", buf);

        // delete
        demanderTexte("Faits supprimes (separes par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "delete:%s\n", buf);
    }

    fclose(fp);
    printf("\nFichier '%s' cree avec succes.\n", nomFichier);
    return 1;
}

void trucDeBase(const char *nomfile) {
    State initial;
    Goal goal;
    Action actions[MAX_ACTIONS];
    int actionCount = 0;

    if(!parseFile(nomfile, &initial, &goal, actions, &actionCount)) {
        // erreur
    }

    // Affichage simple
    printf("\n=== ETAT INITIAL (START) ===\n");
    printf("Nombre de faits: %d\n", initial.factCount);
    for(int i=0; i<initial.factCount; i++){
        printf(" - %s\n", initial.facts[i]);
    }

    printf("\n=== OBJECTIF (FINISH) ===\n");
    printf("Nombre de faits: %d\n", goal.factCount);
    for(int i=0; i<goal.factCount; i++){
        printf(" - %s\n", goal.facts[i]);
    }

    printf("\n=== ACTIONS DISPONIBLES (%d) ===\n", actionCount);
    for(int i=0; i<actionCount; i++){
        // On n'affiche que le nom
        printf(" - %s\n", actions[i].name);
    }

    // Lancement BFS
    int solIndex = bfs(&initial, &goal, actions, actionCount);
    if(solIndex == -1) {
        printf("\nAucune solution trouvee.\n\n");
    } else {
        reconstructPlan(solIndex, actions);
    }
}

void choixUn(){
    // Fichier par defaut : monkeys.txt
    const char* defaultFile = "monkeys.txt";

    trucDeBase(defaultFile);


}

void choixDeux(){
    // Fichier personnalise
    char nomFichier[MAX_LEN];
    printf("Entrez le nom du fichier a utiliser : ");
    scanf("%s", nomFichier);

    // Debug: Print the file name
    printf("Nom du fichier saisi : %s\n", nomFichier);

    // Check if the file exists (optional, depends on your environment)
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL) {
        printf("Erreur : Le fichier %s n'existe pas ou n'est pas accessible.\n", nomFichier);
        return;
    }
    fclose(file);

    trucDeBase(nomFichier);

}

void choixTrois(){
  // Creer un nouveau fichier
            char nomFichier[MAX_LEN];
            if(!creerFichier(nomFichier)) {
               // echec creation
            }
            // Proposition de verification immediate
            printf("Voulez-vous tester la resolution sur ce fichier ? (o/n) ");
            char rep[10];
            fgets(rep, 10, stdin);
            if(rep[0] == 'o' || rep[0] == 'O') {
                // On parse & BFS
                State initial;
                Goal goal;
                Action actions[MAX_ACTIONS];
                int actionCount = 0;

                if(!parseFile(nomFichier, &initial, &goal, actions, &actionCount)) {
                    printf("Erreur lors du parsing. Abandon.\n");
                } else {
                    // Meme affichage
                    printf("\n=== ETAT INITIAL (START) ===\n");
                    printf("Nombre de faits: %d\n", initial.factCount);
                    for(int i=0; i<initial.factCount; i++){
                        printf(" - %s\n", initial.facts[i]);
                    }

                    printf("\n=== OBJECTIF (FINISH) ===\n");
                    printf("Nombre de faits: %d\n", goal.factCount);
                    for(int i=0; i<goal.factCount; i++){
                        printf(" - %s\n", goal.facts[i]);
                    }

                    printf("\n=== ACTIONS DISPONIBLES (%d) ===\n", actionCount);
                    for(int i=0; i<actionCount; i++){
                        printf(" - %s\n", actions[i].name);
                    }

                    int solIndex = bfs(&initial, &goal, actions, actionCount);
                    if(solIndex == -1) {
                        printf("\nAucune solution trouvee pour ce fichier.\n\n");
                    } else {
                        reconstructPlan(solIndex, actions);
                    }
                }
            } else {
                printf("Ok, fichier cree, pas de verification. Retour au menu.\n");
            }
        }

// ---------------------------------------------------------------------
// 7) Programme principal
// ---------------------------------------------------------------------
int main() {
    int choix=0;

    // Affichage ASCII
    afficherEnteteASCII();

    while (choix != 4){
        choix = afficherMenu();

        switch (choix) {
            case 1:
                choixUn();
            break;
            case 2:
                choixDeux();
            break;
            case 3:
                choixTrois();
            break;
            case 4:
                printf("Au revoir !\n");
            break;
            default:
                printf("Choix invalide.\n");
        }
    }

    return 0;
}