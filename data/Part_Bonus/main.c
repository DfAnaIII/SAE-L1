#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

// ---------------------------------------------------------------------
// Configuration / limites
// ---------------------------------------------------------------------
#define MAX_LEN      256   // Taille max d'une ligne
#define MAX_FACTS    50    // Nombre max de "faits" qu'on stocke dans un état (augmenté pour les blocs)
#define MAX_ACTIONS  100   // Nombre max d'actions (augmenté pour les blocs)
#define MAX_STATES   5000  // Nombre max d'états visités lors du BFS (augmenté pour la complexité)
#define MAX_PRIORITY 10    // Priorité maximale d'une règle

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
// On ajoute une priorité pour la partie 5.5
typedef struct {
    char name[MAX_LEN];
    State preconds;
    State addList;
    State delList;
    int priority;   // Priorité de la règle (1-10)
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
int DEBUG_MODE = 1;        // Mode débug activé par défaut

// ---------------------------------------------------------------------
// Définition de constantes pour les modes de recherche (partie 5)
// ---------------------------------------------------------------------
#define MODE_NORMAL 0
#define MODE_MELANGE 1
#define MODE_ALEATOIRE 2
#define MODE_PRIORITE 3
#define MODE_MEANS_END 4

// ---------------------------------------------------------------------
// 1) Affichage ASCII de présentation
// ---------------------------------------------------------------------
void AfficherEnteteASCII() {
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
int AfficherMenu() {
    printf("+---------------------------------------------------+\n");
    printf("|                                                   |\n");
    printf("|                 Menu principal :                  |\n");
    printf("|                                                   |\n");
    printf("|  1) Utiliser le fichier par defaut (monkey.txt)   |\n");
    printf("|  2) Indiquer un fichier personnalise              |\n");
    printf("|  3) Creer un nouveau fichier                      |\n");
    printf("|  4) Partie 5: Gestion de la complexité            |\n");
    printf("|  0) Quitter                                       |\n");
    printf("|                                                   |\n");
    printf("+---------------------------------------------------+\n");
    printf("Votre choix : ");
    int choix;
    scanf("%d", &choix);
    return choix;
}

// Menu pour la partie 5
int AfficherMenuPartie5() {
    printf("+---------------------------------------------------+\n");
    printf("|                                                   |\n");
    printf("|           Partie 5: Gestion de la complexité      |\n");
    printf("|                                                   |\n");
    printf("|  1) Générer un problème de blocs                  |\n");
    printf("|  2) Lancer une recherche sur un problème          |\n");
    printf("|  3) Comparer les différentes stratégies           |\n");
    printf("|  0) Retour au menu principal                      |\n");
    printf("|                                                   |\n");
    printf("+---------------------------------------------------+\n");
    printf("Votre choix : ");
    int choix;
    scanf("%d", &choix);
    return choix;
}

// Menu pour choisir une stratégie de recherche
int AfficherMenuStrategies() {
    printf("+---------------------------------------------------+\n");
    printf("|                                                   |\n");
    printf("|           Choisir la stratégie de recherche       |\n");
    printf("|                                                   |\n");
    printf("|  1) Normal (BFS standard)                         |\n");
    printf("|  2) Mélange des règles                            |\n");
    printf("|  3) Choix aléatoire des règles                    |\n");
    printf("|  4) Priorité des règles                           |\n");
    printf("|  5) Analyse fin-moyens                            |\n");
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
void Trim(char* str) {
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
void SplitFacts(const char* line, State* state) {
    state->factCount = 0;
    char buffer[MAX_LEN];
    strcpy(buffer, line);

    char* token = strtok(buffer, ",");
    while(token != NULL && state->factCount < MAX_FACTS) {
        Trim(token);
        if(strlen(token) > 0) {
            strcpy(state->facts[state->factCount], token);
            state->factCount++;
        }
        token = strtok(NULL, ",");
    }
}

// Verifie que subset est inclus dans st : tous les faits de subset doivent etre dans st
int StateContainsAll(const State* st, const State* subset) {
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
int CanApply(const State* st, const Action* action) {
    return StateContainsAll(st, &action->preconds);
}

// Applique l'action : on supprime delList, puis on ajoute addList (sans doublons)
void ApplyAction(const State* st, const Action* action, State* newState) {
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
bool SameState(const State* a, const State* b) {
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
int IsGoalReached(const State* st, const Goal* goal) {
    return StateContainsAll(st, goal);
}

// ---------------------------------------------------------------------
// 3) Parsing du fichier
// ---------------------------------------------------------------------
// Lecture en 1 passe : start / finish / **** / action:... / preconds:... / add:... / delete:...
int ParseFile(const char* filename, State* initial, Goal* goal, Action* actions, int* actionCount) {
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
                SplitFacts(line + 6, initial);
            }
            else if(strncmp(line, "finish:", 7) == 0) {
                SplitFacts(line + 7, goal);
            }
        } else {
            // On est dans un bloc action
            int idx = readingActionIndex;
            if(strncmp(line, "action:", 7) == 0) {
                strcpy(actions[idx].name, line + 7);
            }
            else if(strncmp(line, "preconds:", 9) == 0) {
                SplitFacts(line + 9, &actions[idx].preconds);
            }
            else if(strncmp(line, "add:", 4) == 0) {
                SplitFacts(line + 4, &actions[idx].addList);
            }
            else if(strncmp(line, "delete:", 7) == 0) {
                SplitFacts(line + 7, &actions[idx].delList);
            }
        }
    }

    fclose(fp);
    return 1;
}

// ---------------------------------------------------------------------
// 4) BFS améliorée : avec support des différents modes de recherche
// ---------------------------------------------------------------------
int BfsAmeliore(const State* start, const Goal* goal, Action* actions, int actionCount, int mode) {
    // Reinit
    visitedCount = 0;
    front = 0;
    rear = 0;

    // Mesure du temps
    struct timeval debutRecherche, finRecherche;
    gettimeofday(&debutRecherche, NULL);
    
    // Mélanger les règles au début si demandé
    if (mode == MODE_MELANGE) {
        if (DEBUG_MODE) printf("DEBUG: Mode MELANGE activé - mélange des règles...\n");
        MelangerRegles(actions, actionCount);
    }

    // L'etat initial -> visited[0]
    visited[0].state = *start;
    visited[0].parent = -1;
    visited[0].actionFromParent = -1;
    visitedCount = 1;

    // On push 0 dans la file
    queueArray[rear++] = 0;
    
    int etatsExplores = 0;

    // Parcours
    while(front < rear) {
        int currentIndex = queueArray[front++];
        Node currentNode = visited[currentIndex];
        etatsExplores++;
        
        if (DEBUG_MODE && etatsExplores % 100 == 0) {
            printf("DEBUG: %d états explorés...\n", etatsExplores);
        }

        // Test objectif
        if(IsGoalReached(&currentNode.state, goal)) {
            gettimeofday(&finRecherche, NULL);
            double tempsRecherche = (finRecherche.tv_sec - debutRecherche.tv_sec) * 1000.0 + 
                                   (finRecherche.tv_usec - debutRecherche.tv_usec) / 1000.0;
            if (DEBUG_MODE) {
                printf("DEBUG: Solution trouvée ! (%d états explorés, %.2f ms)\n", 
                      etatsExplores, tempsRecherche);
            }
            return currentIndex; // On a trouve
        }

        // Selon le mode, on choisit différemment la prochaine règle à appliquer
        switch(mode) {
            case MODE_NORMAL: {
                // Mode normal : on essaie toutes les actions en ordre
                for(int a = 0; a < actionCount; a++) {
                    if(CanApply(&currentNode.state, &actions[a])) {
                        if (DEBUG_MODE) printf("DEBUG: Applique règle %d: %s\n", a, actions[a].name);
                        
                        // generer un nouvel etat
                        State newState;
                        ApplyAction(&currentNode.state, &actions[a], &newState);

                        // verif si deja vu
                        bool dejaVu = false;
                        for(int i = 0; i < visitedCount; i++) {
                            if(SameState(&newState, &visited[i].state)) {
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
                break;
            }
            
            case MODE_ALEATOIRE: {
                // Mode aléatoire : on choisit une règle au hasard parmi les applicables
                int nbApplicables = 0;
                int* applicables = TrouverReglesApplicables(&currentNode.state, actions, actionCount, &nbApplicables);
                
                if (nbApplicables > 0) {
                    // Choisir une règle au hasard
                    int indiceAleatoire = rand() % nbApplicables;
                    int a = applicables[indiceAleatoire];
                    
                    if (DEBUG_MODE) printf("DEBUG: Applique règle aléatoire %d: %s\n", a, actions[a].name);
                    
                    // Génerer un nouvel état
                    State newState;
                    ApplyAction(&currentNode.state, &actions[a], &newState);
                    
                    // Verif si déjà vu
                    bool dejaVu = false;
                    for(int i = 0; i < visitedCount; i++) {
                        if(SameState(&newState, &visited[i].state)) {
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
                break;
            }
            
            case MODE_PRIORITE: {
                // Mode priorité : on utilise le tableau pondéré par les priorités
                int taillePonderee = 0;
                int* tableauPriorite = ConstuireTableauPriorite(&currentNode.state, actions, actionCount, &taillePonderee);
                
                if (taillePonderee > 0) {
                    // Choisir une règle au hasard selon la pondération
                    int indiceAleatoire = rand() % taillePonderee;
                    int a = tableauPriorite[indiceAleatoire];
                    
                    if (DEBUG_MODE) printf("DEBUG: Applique règle par priorité %d: %s (priorité %d)\n", 
                                          a, actions[a].name, actions[a].priority);
                    
                    // Génerer un nouvel état
                    State newState;
                    ApplyAction(&currentNode.state, &actions[a], &newState);
                    
                    // Verif si déjà vu
                    bool dejaVu = false;
                    for(int i = 0; i < visitedCount; i++) {
                        if(SameState(&newState, &visited[i].state)) {
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
                break;
            }
            
            case MODE_MEANS_END: {
                // Mode analyse fin-moyens : on choisit la règle qui nous rapproche le plus du but
                int meilleureRegle = ChoisirMeilleureRegle(&currentNode.state, goal, actions, actionCount);
                
                if (meilleureRegle != -1) {
                    if (DEBUG_MODE) printf("DEBUG: Applique meilleure règle %d: %s\n", 
                                          meilleureRegle, actions[meilleureRegle].name);
                    
                    // Génerer un nouvel état
                    State newState;
                    ApplyAction(&currentNode.state, &actions[meilleureRegle], &newState);
                    
                    // Verif si déjà vu
                    bool dejaVu = false;
                    for(int i = 0; i < visitedCount; i++) {
                        if(SameState(&newState, &visited[i].state)) {
                            dejaVu = true;
                            break;
                        }
                    }
                    if(!dejaVu && visitedCount < MAX_STATES) {
                        visited[visitedCount].state = newState;
                        visited[visitedCount].parent = currentIndex;
                        visited[visitedCount].actionFromParent = meilleureRegle;
                        queueArray[rear++] = visitedCount;
                        visitedCount++;
                    }
                }
                break;
            }
        }
    }

    // Pas trouve
    gettimeofday(&finRecherche, NULL);
    double tempsRecherche = (finRecherche.tv_sec - debutRecherche.tv_sec) * 1000.0 + 
                           (finRecherche.tv_usec - debutRecherche.tv_usec) / 1000.0;
    if (DEBUG_MODE) {
        printf("DEBUG: Aucune solution trouvée. (%d états explorés, %.2f ms)\n", 
              etatsExplores, tempsRecherche);
    }
    return -1;
}

// ---------------------------------------------------------------------
// 5) Reconstruction du plan (liste d'actions) a partir de l'index solution
// ---------------------------------------------------------------------
void ReconstructPlan(int solutionIndex, Action* actions) {
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
    printf("\n+------------------PLAN TROUVE------------------+\n|\n");
    int step = 1;
    // Saute le tout dernier -1
    for(int i = length - 2; i >= 0; i--) {
        int actionIndex = plan[i];
        printf("|  Etape %d: %s \n", step, actions[actionIndex].name);
        step++;
    }
    printf("|\n+------------------FIN DU PLAN------------------+\n\n");
}

// ---------------------------------------------------------------------
// 6) Fonctions pour creation d'un fichier via l'utilisateur
// ---------------------------------------------------------------------

// Pose une question et recupere la reponse dans 'buffer'
void DemanderTexte(const char* question, char* buffer, int bufferSize) {
    printf("%s", question);
    fgets(buffer, bufferSize, stdin);
    // enlever \n
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

// Cree un fichier en format correct
// start:...
// finish:...
// puis un certain nombre d'actions, chacune separee par ****
int CreerFichier(char* nomFichier) {

    printf("Entrez le nom du fichier à créer (ex: monFichier.txt) : ");
    fgets(nomFichier, 255, stdin);

    size_t len = strlen(nomFichier);
    if (len > 0 && nomFichier[len - 1] == '\n') {
        nomFichier[len - 1] = '\0';
    }

    char cheminComplet[512];
    snprintf(cheminComplet, sizeof(cheminComplet), "assets/%s", nomFichier);

    FILE* fp = fopen(cheminComplet, "w");
    if (!fp) {
        printf("Impossible de créer le fichier '%s'.\n", cheminComplet);
        return 0;
    }

    char buf[MAX_LEN];
    DemanderTexte("Entrez les conditions initiales (séparées par des virgules):\n> ", buf, MAX_LEN);
    fprintf(fp, "start:%s\n", buf);

    DemanderTexte("Entrez les conditions d'arrivée (finish), séparées par des virgules:\n> ", buf, MAX_LEN);
    fprintf(fp, "finish:%s\n", buf);

    int nbActions = 0;
    printf("Combien d'actions voulez-vous saisir ? ");
    scanf("%d", &nbActions);
    getchar();

    for (int i = 0; i < nbActions; i++) {
        fprintf(fp, "****\n");
        // Action (nom)
        DemanderTexte("\nNom de l'action : ", buf, MAX_LEN);
        fprintf(fp, "action:%s\n", buf);

        // Préconditions
        DemanderTexte("Préconditions (séparées par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "preconds:%s\n", buf);

        // Faits ajoutés
        DemanderTexte("Faits ajoutés (séparés par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "add:%s\n", buf);

        // Faits supprimés
        DemanderTexte("Faits supprimés (séparés par des virgules) : ", buf, MAX_LEN);
        fprintf(fp, "delete:%s\n", buf);
    }

    fclose(fp);
    printf("\nFichier '%s' créé avec succès dans le dossier 'assets'.\n", cheminComplet);
    return 1;
}

// Initialiser les priorités des règles (pour la partie 5.5)
void InitialiserPriorites(Action* actions, int actionCount) {
    // Par défaut, toutes les règles ont une priorité de 1
    for (int i = 0; i < actionCount; i++) {
        actions[i].priority = 1;
    }
    
    // Pour le problème des blocs, on peut mettre une priorité plus élevée 
    // aux règles qui posent les blocs sur la table
    for (int i = 0; i < actionCount; i++) {
        if (strstr(actions[i].name, "sur la table") != NULL) {
            actions[i].priority = 5;  // Priorité élevée
            if (DEBUG_MODE) printf("DEBUG: Priorité 5 pour règle %d: %s\n", i, actions[i].name);
        }
    }
}

// Version améliorée pour la partie 5
void AnalyseFichierAvecStrategie(const char *nomfile, int mode) {
    State initial;
    Goal goal;
    Action actions[MAX_ACTIONS];
    int actionCount = 0;
    
    if (DEBUG_MODE) printf("DEBUG: Analyse du fichier '%s' avec mode %d\n", nomfile, mode);

    if(!ParseFile(nomfile, &initial, &goal, actions, &actionCount)) {
        printf("Erreur lors du chargement du fichier.\n");
        return;
    }
    
    // Initialiser les priorités pour le mode priorité
    if (mode == MODE_PRIORITE) {
        InitialiserPriorites(actions, actionCount);
    }

    // Affichage simple
    printf("\n+----------ETAT INITIAL (START)----------+\n|\n");
    printf("| Nombre de faits: %d\n", initial.factCount);
    for(int i=0; i<initial.factCount; i++){
        printf("| - %s\n", initial.facts[i]);
    }
    printf("|\n+----------------------------------------+\n");


    printf("\n+----------OBJECTIF (FINISH)----------+\n|\n");
    printf("| Nombre de faits: %d\n", goal.factCount);
    for(int i=0; i<goal.factCount; i++){
        printf("| - %s\n", goal.facts[i]);
    }
    printf("|\n+-------------------------------------+\n");


    printf("\n+----------ACTIONS DISPONIBLES (%d)----------+\n|\n", actionCount);
    for(int i=0; i<actionCount; i++){
        printf("| - %s", actions[i].name);
        if (mode == MODE_PRIORITE) {
            printf(" (priorité: %d)", actions[i].priority);
        }
        printf("\n");
    }
    printf("|\n+-------------------------------------------+\n");
    
    // Affichage du mode de recherche
    printf("\n+----------STRATEGIE DE RECHERCHE----------+\n|\n");
    switch(mode) {
        case MODE_NORMAL:
            printf("| Mode: NORMAL (BFS standard)\n");
            break;
        case MODE_MELANGE:
            printf("| Mode: MELANGE DES REGLES\n");
            break;
        case MODE_ALEATOIRE:
            printf("| Mode: CHOIX ALEATOIRE\n");
            break;
        case MODE_PRIORITE:
            printf("| Mode: PRIORITE DES REGLES\n");
            break;
        case MODE_MEANS_END:
            printf("| Mode: ANALYSE FIN-MOYENS\n");
            break;
    }
    printf("|\n+------------------------------------------+\n");

    // Mesure du temps d'exécution
    struct timeval debut, fin;
    gettimeofday(&debut, NULL);
    
    // Lancement BFS amélioré
    int solIndex = BfsAmeliore(&initial, &goal, actions, actionCount, mode);
    
    gettimeofday(&fin, NULL);
    double temps_ms = (fin.tv_sec - debut.tv_sec) * 1000.0 + (fin.tv_usec - debut.tv_usec) / 1000.0;
    
    if(solIndex == -1) {
        printf("\n+===========================================================+\n");
        printf("|                  AUCUNE SOLUTION TROUVÉE                  |\n");
        printf("+===========================================================+\n");
    } else {
        ReconstructPlan(solIndex, actions);
    }
    
    printf("\n+----------STATISTIQUES----------+\n");
    printf("| Nombre d'états générés: %d\n", visitedCount);
    printf("| Temps d'exécution: %.2f ms\n", temps_ms);
    printf("+--------------------------------+\n");
}

void ChoixFichierParDefaut(){
    // Fichier par defaut : monkey.txt
    const char* defaultFile = "assets/monkey.txt";

    // Appele de fonction
    AnalyseFichierAvecStrategie(defaultFile, MODE_NORMAL);
}

void viderbuffer()
{
    int c;
    while ((c=getchar())!=EOF && c!='\n');
}

void ChoixFichierUtilisateur(){
    char * nomFichier = malloc (sizeof(char)*255);
    if (nomFichier==NULL){
        fprintf(stderr, "Erreur  d'initialisation du nom de ficher\n" );
        return;
    }

    char chemin[512];

    printf("Entrez le nom du fichier a utiliser : ");
    viderbuffer();
    scanf("%254s", nomFichier);

    strcpy(chemin, "assets/");
    strcat(chemin, nomFichier);


    printf("Nom du fichier saisi : %s\n", chemin);

    FILE *file = fopen(chemin, "r");
    if (file == NULL) {
        printf("Erreur : Le fichier %s n'existe pas ou n'est pas accessible.\n", chemin);
        free(nomFichier);
        return;
    }
    fclose(file);

    AnalyseFichierAvecStrategie(chemin, MODE_NORMAL);

    free(nomFichier);
}


void ChoixCréationFichierUtilisateur(){
    // Creer un nouveau fichier
    char *nomFichier = malloc(sizeof(char) * 255);

    // Demander le nom du fichier
    printf("Nom du fichier à créer : ");
    fgets(nomFichier, 255, stdin);

    // Supprimer le \n de la fin de la chaîne si présent
    size_t len = strlen(nomFichier);
    if (len > 0 && nomFichier[len - 1] == '\n') {
        nomFichier[len - 1] = '\0';
    }

    // Créer le fichier
    CreerFichier(nomFichier);

    // Proposition de vérification immédiate
    printf("Voulez-vous tester la résolution sur ce fichier ? (o/n) ");
    char rep[10];
    fgets(rep, 10, stdin);

    if (rep[0] == 'o' || rep[0] == 'O') {
        // On parse & BFS
        State initial;
        Goal goal;
        Action actions[MAX_ACTIONS];
        int actionCount = 0;

        if (!ParseFile(nomFichier, &initial, &goal, actions, &actionCount)) {
            printf("Erreur lors du parsing. Abandon.\n");
        } else {
            // Même affichage
            printf("\n=== ETAT INITIAL (START) ===\n");
            printf("Nombre de faits: %d\n", initial.factCount);
            for (int i = 0; i < initial.factCount; i++) {
                printf(" - %s\n", initial.facts[i]);
            }

            printf("\n=== OBJECTIF (FINISH) ===\n");
            printf("Nombre de faits: %d\n", goal.factCount);
            for (int i = 0; i < goal.factCount; i++) {
                printf(" - %s\n", goal.facts[i]);
            }

            printf("\n=== ACTIONS DISPONIBLES (%d) ===\n", actionCount);
            for (int i = 0; i < actionCount; i++) {
                printf(" - %s\n", actions[i].name);
            }

            int solIndex = BfsAmeliore(&initial, &goal, actions, actionCount, MODE_NORMAL);
            if (solIndex == -1) {
                printf("\nAucune solution trouvée pour ce fichier.\n\n");
            } else {
                ReconstructPlan(solIndex, actions);
            }
        }
    } else {
        printf("Ok, fichier créé, pas de vérification. Retour au menu.\n");
    }

    free(nomFichier);  // Ne pas oublier de libérer la mémoire allouée
}

// ---------------------------------------------------------------------
// Fonctions pour la partie 5
// ---------------------------------------------------------------------

// 5.1 - Génère un fichier avec des règles pour déplacer n blocs empilés
void GenererProblemeBlocs(int nblocs) {
    if (DEBUG_MODE) printf("DEBUG: Génération d'un problème à %d blocs...\n", nblocs);
    
    char filename[MAX_LEN];
    sprintf(filename, "assets/blocs_%d.txt", nblocs);
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Erreur: impossible de créer le fichier %s\n", filename);
        return;
    }
    
    // État initial : tous les blocs sont empilés dans l'ordre (bloc_1 est au sommet)
    fprintf(fp, "start:");
    for (int i = 1; i <= nblocs; i++) {
        if (i > 1) {
            fprintf(fp, "sur(bloc_%d,bloc_%d)", i-1, i);
            if (i < nblocs) fprintf(fp, ",");
        }
    }
    fprintf(fp, ",sur(bloc_%d,table)", nblocs);
    for (int i = 1; i <= nblocs; i++) {
        fprintf(fp, ",libre(bloc_%d)", i==1 ? i : 0);  // seul le bloc du dessus est libre
    }
    fprintf(fp, "\n");
    
    // État final : tous les blocs sont sur la table
    fprintf(fp, "finish:");
    for (int i = 1; i <= nblocs; i++) {
        fprintf(fp, "sur(bloc_%d,table)", i);
        if (i < nblocs) fprintf(fp, ",");
    }
    for (int i = 1; i <= nblocs; i++) {
        fprintf(fp, ",libre(bloc_%d)", i);  // tous les blocs sont libres
    }
    fprintf(fp, "\n");
    
    // Règles pour déplacer un bloc du sommet vers la table
    for (int i = 1; i <= nblocs; i++) {
        fprintf(fp, "****\n");
        fprintf(fp, "action:Poser bloc_%d sur la table\n", i);
        fprintf(fp, "preconds:libre(bloc_%d),sur(bloc_%d,bloc_X)\n", i, i);
        fprintf(fp, "add:sur(bloc_%d,table),libre(bloc_X)\n", i);
        fprintf(fp, "delete:sur(bloc_%d,bloc_X)\n", i);
    }
    
    // Règles pour déplacer un bloc du sommet vers un autre bloc
    for (int i = 1; i <= nblocs; i++) {
        for (int j = 1; j <= nblocs; j++) {
            if (i != j) {
                fprintf(fp, "****\n");
                fprintf(fp, "action:Déplacer bloc_%d sur bloc_%d\n", i, j);
                fprintf(fp, "preconds:libre(bloc_%d),libre(bloc_%d),sur(bloc_%d,bloc_X)\n", i, j, i);
                fprintf(fp, "add:sur(bloc_%d,bloc_%d),libre(bloc_X)\n", i, j);
                fprintf(fp, "delete:sur(bloc_%d,bloc_X),libre(bloc_%d)\n", i, j);
            }
        }
    }
    
    fclose(fp);
    printf("Fichier '%s' créé avec succès.\n", filename);
}

// 5.3 - Mélange l'ordre des règles (pour augmenter les chances de trouver une solution rapidement)
void MelangerRegles(Action* actions, int actionCount) {
    if (DEBUG_MODE) printf("DEBUG: Mélange de %d règles...\n", actionCount);
    
    srand(time(NULL));
    for (int i = 0; i < actionCount; i++) {
        int j = i + rand() % (actionCount - i);
        // Échange les règles i et j
        Action temp = actions[i];
        actions[i] = actions[j];
        actions[j] = temp;
    }
}

// 5.4 - Choix aléatoire parmi les règles applicables
int* TrouverReglesApplicables(const State* state, Action* actions, int actionCount, int* nbApplicables) {
    static int applicables[MAX_ACTIONS];
    *nbApplicables = 0;
    
    for (int i = 0; i < actionCount; i++) {
        if (CanApply(state, &actions[i])) {
            applicables[(*nbApplicables)++] = i;
        }
    }
    
    return applicables;
}

// 5.5 - Construction d'un tableau de règles applicables pondéré par les priorités
int* ConstuireTableauPriorite(const State* state, Action* actions, int actionCount, int* tailleTableau) {
    static int tableauPonderé[MAX_ACTIONS * MAX_PRIORITY];
    *tailleTableau = 0;
    
    for (int i = 0; i < actionCount; i++) {
        if (CanApply(state, &actions[i])) {
            for (int p = 0; p < actions[i].priority; p++) {
                tableauPonderé[(*tailleTableau)++] = i;
            }
        }
    }
    
    return tableauPonderé;
}

// 5.6 - Calcul de la distance par rapport au but (nombre de faits communs)
int CalculerDistanceAuBut(const State* state, const Goal* goal) {
    int nbFaitsCommuns = 0;
    
    for (int i = 0; i < goal->factCount; i++) {
        for (int j = 0; j < state->factCount; j++) {
            if (strcmp(goal->facts[i], state->facts[j]) == 0) {
                nbFaitsCommuns++;
                break;
            }
        }
    }
    
    return nbFaitsCommuns;
}

// Choisit la meilleure règle selon l'analyse fin-moyens
int ChoisirMeilleureRegle(const State* state, const Goal* goal, Action* actions, int actionCount) {
    int meilleureRegle = -1;
    int meilleurScore = -1;
    
    for (int i = 0; i < actionCount; i++) {
        if (CanApply(state, &actions[i])) {
            // Calcul de l'état résultant
            State newState;
            ApplyAction(state, &actions[i], &newState);
            
            // Calcul du score (nombre de faits communs avec le but)
            int score = CalculerDistanceAuBut(&newState, goal);
            
            if (score > meilleurScore) {
                meilleurScore = score;
                meilleureRegle = i;
            }
        }
    }
    
    return meilleureRegle;
}

// ---------------------------------------------------------------------
// Gestion des menus pour la partie 5
// ---------------------------------------------------------------------

void MenuGenerationBlocs() {
    int nblocs;
    printf("Entrez le nombre de blocs à générer (4-10 recommandé) : ");
    scanf("%d", &nblocs);
    
    if (nblocs < 2 || nblocs > 20) {
        printf("Nombre de blocs invalide. Doit être entre 2 et 20.\n");
        return;
    }
    
    GenererProblemeBlocs(nblocs);
}

void MenuRechercheProbleme() {
    char nomFichier[MAX_LEN];
    printf("Entrez le nom du fichier à utiliser : ");
    scanf("%s", nomFichier);
    
    char chemin[MAX_LEN * 2];
    snprintf(chemin, sizeof(chemin), "assets/%s", nomFichier);
    
    FILE *file = fopen(chemin, "r");
    if (file == NULL) {
        printf("Erreur : Le fichier %s n'existe pas ou n'est pas accessible.\n", chemin);
        return;
    }
    fclose(file);
    
    int strategie = AfficherMenuStrategies();
    if (strategie < 1 || strategie > 5) {
        printf("Stratégie invalide.\n");
        return;
    }
    
    // Conversion du choix en mode
    int mode;
    switch(strategie) {
        case 1: mode = MODE_NORMAL; break;
        case 2: mode = MODE_MELANGE; break;
        case 3: mode = MODE_ALEATOIRE; break;
        case 4: mode = MODE_PRIORITE; break;
        case 5: mode = MODE_MEANS_END; break;
        default: mode = MODE_NORMAL;
    }
    
    AnalyseFichierAvecStrategie(chemin, mode);
}

void MenuComparaison() {
    char nomFichier[MAX_LEN];
    printf("Entrez le nom du fichier à utiliser pour la comparaison : ");
    scanf("%s", nomFichier);
    
    char chemin[MAX_LEN * 2];
    snprintf(chemin, sizeof(chemin), "assets/%s", nomFichier);
    
    FILE *file = fopen(chemin, "r");
    if (file == NULL) {
        printf("Erreur : Le fichier %s n'existe pas ou n'est pas accessible.\n", chemin);
        return;
    }
    fclose(file);
    
    printf("\n+----------COMPARAISON DES STRATEGIES----------+\n");
    printf("| Exécution de toutes les stratégies sur %s\n", nomFichier);
    printf("+-------------------------------------------+\n\n");
    
    int modeDebugOrig = DEBUG_MODE;
    DEBUG_MODE = 0;  // Désactiver temporairement le mode debug pour éviter trop d'affichage
    
    const char* nomModes[] = {"NORMAL", "MELANGE", "ALEATOIRE", "PRIORITE", "MEANS-END"};
    
    for (int mode = MODE_NORMAL; mode <= MODE_MEANS_END; mode++) {
        printf("\nStratégie: %s\n", nomModes[mode]);
        
        struct timeval debut, fin;
        gettimeofday(&debut, NULL);
        
        State initial;
        Goal goal;
        Action actions[MAX_ACTIONS];
        int actionCount = 0;
        
        if (ParseFile(chemin, &initial, &goal, actions, &actionCount)) {
            if (mode == MODE_PRIORITE) {
                InitialiserPriorites(actions, actionCount);
            }
            
            int solIndex = BfsAmeliore(&initial, &goal, actions, actionCount, mode);
            
            gettimeofday(&fin, NULL);
            double temps_ms = (fin.tv_sec - debut.tv_sec) * 1000.0 + (fin.tv_usec - debut.tv_usec) / 1000.0;
            
            printf("  Résultat: %s\n", solIndex >= 0 ? "Solution trouvée" : "Échec");
            printf("  États générés: %d\n", visitedCount);
            printf("  Temps: %.2f ms\n", temps_ms);
            
            if (solIndex >= 0) {
                // Calculer la longueur du plan
                int longueur = 0;
                int current = solIndex;
                while (current > 0) {
                    longueur++;
                    current = visited[current].parent;
                }
                printf("  Longueur du plan: %d étapes\n", longueur);
            }
        } else {
            printf("  Erreur lors du chargement du fichier.\n");
        }
    }
    
    DEBUG_MODE = modeDebugOrig;  // Restaurer le mode debug original
}

void GestionPartie5() {
    int choix = -1;
    
    while (choix != 0) {
        choix = AfficherMenuPartie5();
        
        switch (choix) {
            case 0:
                printf("Retour au menu principal.\n");
                break;
            case 1:
                MenuGenerationBlocs();
                break;
            case 2:
                MenuRechercheProbleme();
                break;
            case 3:
                MenuComparaison();
                break;
            default:
                printf("Choix invalide.\n");
        }
        
        if (choix != 0) {
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();  // Pour capturer l'entrée précédente
            getchar();  // Pour attendre l'appui sur Entrée
        }
    }
}

// ---------------------------------------------------------------------
// 7) Programme principal
// ---------------------------------------------------------------------
int main() {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    int choix = -1;

    AfficherEnteteASCII();

    while (choix != 0) {
        choix = AfficherMenu();

        switch (choix) {
            case 0:
                printf("Au revoir !\n");
                break;
            case 1:
                ChoixFichierParDefaut();
                break;
            case 2:
                ChoixFichierUtilisateur();
                break;
            case 3:
                ChoixCréationFichierUtilisateur();
                break;
            case 4:
                GestionPartie5();
                break;
            default:
                printf("Choix invalide.\n");
        }
        
        if (choix != 0) {
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();  // Pour capturer l'entrée précédente
            getchar();  // Pour attendre l'appui sur Entrée
        }
    }

    return 0;
}