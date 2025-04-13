#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LIGNE 256
#define MAX_MOTS 50
#define MAX_REGLES 100
#define MAX_ETATS 1000
#define MAX_CHEMIN 100

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
 * Affiche tous les faits d'un état 
 */
void afficherEtat(Etat *etat) {
    printf("[ ");
    for(int i = 0; i < etat->nb_faits; i++) {
        printf("%s", etat->faits[i]);
        if(i < etat->nb_faits - 1) printf(", ");
    }
    printf(" ]");
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
 * Recherche avec backtracking pour trouver une solution - Version avec débogage détaillé
 */
int rechercheSolutionBacktrackDebug(Etat etats[], int *nb_etats, Regle regles[], int nb_regles, 
                             string buts[], int nb_buts) {
    int possible = 1;
    int IR = 0;  // Indice à partir duquel on cherche une règle applicable
    int etat_courant = 0;
    int max_profondeur = 100; // Limite de profondeur pour éviter les boucles infinies
    int profondeur_actuelle = 0;
    int nb_backtrackings = 0;
    int nb_etats_explores = 0;
    
    printf("\n===== DÉBUT DE LA RECHERCHE AVEC BACKTRACKING =====\n\n");
    
    while(possible && !butsAtteints(&etats[etat_courant], buts, nb_buts) && profondeur_actuelle < max_profondeur) {
        int regle_trouvee = 0;
        nb_etats_explores++;
        
        // Afficher l'état courant
        printf("ÉTAT COURANT #%d (profondeur %d):\n", etat_courant, profondeur_actuelle);
        printf("  Faits: ");
        afficherEtat(&etats[etat_courant]);
        printf("\n");
        
        // Vérification des buts
        printf("  Vérification des buts: ");
        for(int i = 0; i < nb_buts; i++) {
            if(contientFait(&etats[etat_courant], buts[i])) {
                printf("'%s' ✓ ", buts[i]);
            } else {
                printf("'%s' ✗ ", buts[i]);
            }
        }
        printf("\n");
        
        // Sécurité : vérifier si on a atteint la limite du tableau des états
        if(*nb_etats >= MAX_ETATS - 1) {
            printf("\n⚠️ ALERTE: Limite du nombre d'états atteinte (%d). Arrêt de la recherche.\n", MAX_ETATS);
            return -1;
        }
        
        printf("  Recherche de règles applicables à partir de l'indice IR=%d\n", IR);
        
        // Cherche une règle applicable d'indice > IR
        for(int i = IR; i < nb_regles; i++) {
            printf("    Évaluation de la règle #%d '%s': ", i, regles[i].action);
            
            // Afficher les préconditions
            printf("préconditions ");
            for(int j = 0; j < regles[i].nb_preconds; j++) {
                const char* precond = regles[i].preconds[j];
                if(contientFait(&etats[etat_courant], precond)) {
                    printf("'%s' ✓ ", precond);
                } else {
                    printf("'%s' ✗ ", precond);
                }
            }
            
            if(preconditionsSatisfaites(&regles[i], &etats[etat_courant])) {
                printf("→ APPLICABLE\n");
                
                // Applique la règle pour créer un nouvel état
                etats[*nb_etats].parent = etat_courant;
                appliquerRegle(&regles[i], &etats[etat_courant], &etats[*nb_etats]);
                
                // Vérifier si le nouvel état est identique à un état déjà visité
                int etat_deja_visite = 0;
                for(int j = 0; j < *nb_etats; j++) {
                    int identique = 1;
                    if(etats[j].nb_faits != etats[*nb_etats].nb_faits) {
                        identique = 0;
                    } else {
                        for(int k = 0; k < etats[j].nb_faits; k++) {
                            int trouve = 0;
                            for(int l = 0; l < etats[*nb_etats].nb_faits; l++) {
                                if(strcmp(etats[j].faits[k], etats[*nb_etats].faits[l]) == 0) {
                                    trouve = 1;
                                    break;
                                }
                            }
                            if(!trouve) {
                                identique = 0;
                                break;
                            }
                        }
                    }
                    
                    if(identique) {
                        etat_deja_visite = 1;
                        break;
                    }
                }
                
                if(etat_deja_visite) {
                    printf("    ⚠️ Le nouvel état est identique à un état déjà visité → IGNORÉ\n");
                } else {
                    printf("    Application de la règle '%s':\n", regles[i].action);
                    printf("      Faits supprimés: ");
                    for(int j = 0; j < regles[i].nb_deletes; j++) {
                        printf("'%s' ", regles[i].deletes[j]);
                    }
                    printf("\n");
                    
                    printf("      Faits ajoutés: ");
                    for(int j = 0; j < regles[i].nb_adds; j++) {
                        printf("'%s' ", regles[i].adds[j]);
                    }
                    printf("\n");
                    
                    printf("      NOUVEL ÉTAT #%d: ", *nb_etats);
                    afficherEtat(&etats[*nb_etats]);
                    printf("\n");
                    
                    etat_courant = *nb_etats;
                    (*nb_etats)++;
                    profondeur_actuelle++;
                    IR = 0;  // Réinitialise IR pour le nouvel état
                    printf("      IR réinitialisé à 0 pour le nouvel état\n");
                    regle_trouvee = 1;
                    break;
                }
            } else {
                printf("→ NON APPLICABLE\n");
            }
        }
        
        if(!regle_trouvee) {
            printf("  ❌ Aucune règle applicable trouvée pour l'état #%d\n", etat_courant);
            
            if(etat_courant > 0) {
                // Backtrack: revient à l'état précédent
                nb_backtrackings++;
                int ancien_etat = etat_courant;
                IR = etats[etat_courant].regle_appliquee + 1;
                etat_courant = etats[etat_courant].parent;
                profondeur_actuelle--;
                
                printf("  ⬅️ BACKTRACKING #%d: Retour à l'état parent #%d\n", nb_backtrackings, etat_courant);
                printf("  IR mis à %d (règle suivante à tester)\n", IR);
            } else {
                possible = 0;  // Aucune solution trouvée
                printf("  ⚠️ Impossible de backtracker depuis l'état initial → ÉCHEC\n");
            }
        }
        
        printf("\n");
    }
    
    if(profondeur_actuelle >= max_profondeur) {
        printf("\n⚠️ ALERTE: Profondeur maximale atteinte (%d). La recherche a été arrêtée.\n", max_profondeur);
        return -1;
    }
    
    printf("===== FIN DE LA RECHERCHE =====\n");
    printf("Nombre total d'états générés: %d\n", *nb_etats);
    printf("Nombre d'états explorés: %d\n", nb_etats_explores);
    printf("Nombre de backtrackings effectués: %d\n", nb_backtrackings);
    
    if(possible && butsAtteints(&etats[etat_courant], buts, nb_buts)) {
        printf("✅ SUCCÈS: Tous les buts sont atteints dans l'état #%d\n\n", etat_courant);
    } else {
        printf("❌ ÉCHEC: Aucune solution trouvée\n\n");
    }
    
    return possible ? etat_courant : -1;
}

/**
 * Affiche les détails d'une règle pour le debugging
 */
void afficherRegleDetail(Regle *regle) {
    printf("  Règle '%s' (#%d):\n", regle->action, regle->indice);
    
    printf("    Préconditions: ");
    for(int i = 0; i < regle->nb_preconds; i++) {
        printf("'%s'", regle->preconds[i]);
        if(i < regle->nb_preconds - 1) printf(", ");
    }
    printf("\n");
    
    printf("    Ajouts: ");
    for(int i = 0; i < regle->nb_adds; i++) {
        printf("'%s'", regle->adds[i]);
        if(i < regle->nb_adds - 1) printf(", ");
    }
    printf("\n");
    
    printf("    Suppressions: ");
    for(int i = 0; i < regle->nb_deletes; i++) {
        printf("'%s'", regle->deletes[i]);
        if(i < regle->nb_deletes - 1) printf(", ");
    }
    printf("\n");
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
    
    // Si longueur = 0, aucune étape n'a été trouvée
    if(longueur == 0) {
        printf("\nAucune solution trouvée. Les buts étaient peut-être déjà satisfaits dans l'état initial.\n");
        return;
    }
    
    printf("\n+------------------PLAN TROUVE------------------+\n|\n");
    printf("| Solution trouvée (%d étapes):\n", longueur);
    for(int i = longueur - 1; i >= 0; i--) {
        int regle = etats[chemin[i]].regle_appliquee;
        printf("| - %s\n", regles[regle].action);
    }
    printf("|\n+------------------FIN DU PLAN------------------+\n\n");
}

/**
 * Affiche la solution trouvée avec des détails supplémentaires
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
    
    // Si longueur = 0, aucune étape n'a été trouvée
    if(longueur == 0) {
        printf("\nAucune solution trouvée. Les buts étaient peut-être déjà satisfaits dans l'état initial.\n");
        return;
    }
    
    printf("\n===== SOLUTION DÉTAILLÉE =====\n\n");
    printf("État initial:\n");
    afficherEtat(&etats[0]);
    printf("\n\n");
    
    for(int i = longueur - 1; i >= 0; i--) {
        int etat_index = chemin[i];
        int regle_index = etats[etat_index].regle_appliquee;
        
        printf("ÉTAPE %d:\n", longueur - i);
        printf("  Application de la règle '%s'\n", regles[regle_index].action);
        
        // Afficher les préconditions
        printf("  Préconditions qui étaient satisfaites:\n");
        for(int j = 0; j < regles[regle_index].nb_preconds; j++) {
            printf("    - %s\n", regles[regle_index].preconds[j]);
        }
        
        // Afficher les suppressions
        printf("  Faits supprimés:\n");
        if(regles[regle_index].nb_deletes == 0) {
            printf("    (aucun)\n");
        } else {
            for(int j = 0; j < regles[regle_index].nb_deletes; j++) {
                printf("    - %s\n", regles[regle_index].deletes[j]);
            }
        }
        
        // Afficher les ajouts
        printf("  Faits ajoutés:\n");
        if(regles[regle_index].nb_adds == 0) {
            printf("    (aucun)\n");
        } else {
            for(int j = 0; j < regles[regle_index].nb_adds; j++) {
                printf("    - %s\n", regles[regle_index].adds[j]);
            }
        }
        
        // Afficher le nouvel état
        printf("  Nouvel état obtenu:\n    ");
        afficherEtat(&etats[etat_index]);
        printf("\n\n");
    }
    
    printf("===== FIN DE LA SOLUTION DÉTAILLÉE =====\n\n");
}

/**
 * Fonction pour diviser une chaîne en utilisant le délimiteur et supprimer les espaces
 */
void diviserChaine(const char *chaine, char delim, string resultat[], int *nb_elements) {
    *nb_elements = 0;
    char buffer[MAX_LIGNE];
    strcpy(buffer, chaine);
    
    char *token = strtok(buffer, &delim);
    while(token != NULL && *nb_elements < MAX_MOTS) {
        // Supprimer les espaces en début et fin
        char *debut = token;
        while(*debut == ' ' || *debut == '\t') debut++;
        
        char *fin = token + strlen(token) - 1;
        while(fin > debut && (*fin == ' ' || *fin == '\t')) {
            *fin = '\0';
            fin--;
        }
        
        if(strlen(debut) > 0) {
            strcpy(resultat[(*nb_elements)++], debut);
        }
        
        token = strtok(NULL, &delim);
    }
}

/**
 * Fonction pour charger un fichier au format spécifié
 */
int chargerFichierFormat(const char *fichier, string faits_initiaux[], int *nb_faits, 
                         string buts[], int *nb_buts, Regle regles[], int *nb_regles) {
    FILE *f = fopen(fichier, "r");
    if(!f) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", fichier);
        return 0;
    }
    
    char ligne[MAX_LIGNE];
    *nb_regles = 0;
    *nb_faits = 0;
    *nb_buts = 0;
    
    Regle regle_courante;
    int dans_action = 0;
    
    while(fgets(ligne, MAX_LIGNE, f)) {
        // Supprimer le retour à la ligne
        ligne[strcspn(ligne, "\r\n")] = 0;
        
        // Ligne vide ou trop courte
        if(strlen(ligne) <= 1) continue;
        
        // Séparateur d'actions
        if(strncmp(ligne, "****", 4) == 0) {
            if(dans_action) {
                // Enregistrer la règle précédente
                regles[(*nb_regles)++] = regle_courante;
            }
            dans_action = 1;
            // Initialiser une nouvelle règle
            memset(&regle_courante, 0, sizeof(Regle));
            continue;
        }
        
        // Parser selon le type de ligne
        if(strncmp(ligne, "start:", 6) == 0) {
            diviserChaine(ligne + 6, ',', faits_initiaux, nb_faits);
        }
        else if(strncmp(ligne, "finish:", 7) == 0) {
            diviserChaine(ligne + 7, ',', buts, nb_buts);
        }
        else if(dans_action) {
            if(strncmp(ligne, "action:", 7) == 0) {
                strcpy(regle_courante.action, ligne + 7);
            }
            else if(strncmp(ligne, "preconds:", 9) == 0) {
                diviserChaine(ligne + 9, ',', regle_courante.preconds, &regle_courante.nb_preconds);
            }
            else if(strncmp(ligne, "add:", 4) == 0) {
                diviserChaine(ligne + 4, ',', regle_courante.adds, &regle_courante.nb_adds);
            }
            else if(strncmp(ligne, "delete:", 7) == 0) {
                diviserChaine(ligne + 7, ',', regle_courante.deletes, &regle_courante.nb_deletes);
            }
        }
    }
    
    // Enregistrer la dernière règle si elle existe
    if(dans_action) {
        regles[(*nb_regles)++] = regle_courante;
    }
    
    fclose(f);
    return 1;
}

/**
 * Fonction pour afficher l'entête ASCII
 */
void afficherEnteteASCII() {
    printf("+----------------------------------------------+\n");
    printf("|                                              |\n");
    printf("|   GPS avec Backtracking (VERSION DÉBOGAGE)   |\n");
    printf("|                                              |\n");
    printf("+----------------------------------------------+\n\n");
}

/**
 * Fonction pour afficher le menu principal
 */
int afficherMenu() {
    printf("\n+---------------------------------------------------+\n");
    printf("|                                                   |\n");
    printf("|                 Menu principal :                  |\n");
    printf("|                                                   |\n");
    printf("|  1) Utiliser le fichier par défaut (monkey.txt)   |\n");
    printf("|  2) Indiquer un fichier personnalisé              |\n");
    printf("|  0) Quitter                                       |\n");
    printf("|                                                   |\n");
    printf("+---------------------------------------------------+\n");
    printf("Votre choix : ");
    int choix;
    scanf("%d", &choix);
    getchar(); // Pour capturer le retour à la ligne
    return choix;
}

/**
 * Fonction pour vider le buffer d'entrée
 */
void viderBuffer() {
    int c;
    while ((c = getchar()) != EOF && c != '\n');
}

/**
 * Fonction pour analyser et résoudre un problème depuis un fichier avec débogage
 */
void analyseFichierDebug(const char *nomFichier) {
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Regle regles[MAX_REGLES];
    int nb_faits = 0, nb_buts = 0, nb_regles = 0;
    
    printf("DEBUG: Chargement du fichier '%s'...\n", nomFichier);
    
    if(!chargerFichierFormat(nomFichier, faits_initiaux, &nb_faits, buts, &nb_buts, regles, &nb_regles)) {
        return; // Erreur déjà affichée
    }
    
    printf("DEBUG: Fichier chargé avec succès!\n");
    
    // Affichage des faits initiaux
    printf("\n+----------ETAT INITIAL (START)----------+\n|\n");
    printf("| Nombre de faits: %d\n", nb_faits);
    for(int i = 0; i < nb_faits; i++) {
        printf("| - %s\n", faits_initiaux[i]);
    }
    printf("|\n+----------------------------------------+\n");
    
    // Affichage des buts
    printf("\n+----------OBJECTIF (FINISH)----------+\n|\n");
    printf("| Nombre de faits: %d\n", nb_buts);
    for(int i = 0; i < nb_buts; i++) {
        printf("| - %s\n", buts[i]);
    }
    printf("|\n+-------------------------------------+\n");
    
    // Affichage des actions
    printf("\n+----------ACTIONS DISPONIBLES (%d)----------+\n|\n", nb_regles);
    for(int i = 0; i < nb_regles; i++) {
        printf("| - %s\n", regles[i].action);
    }
    printf("|\n+-------------------------------------------+\n");
    
    // Affichage détaillé des règles pour le debug
    printf("\n===== DÉTAIL DES RÈGLES =====\n");
    for(int i = 0; i < nb_regles; i++) {
        afficherRegleDetail(&regles[i]);
    }
    printf("\n=============================\n");
    
    // Initialisation des indices des règles
    for(int i = 0; i < nb_regles; i++) {
        regles[i].indice = i;
    }
    
    // Initialisation de l'état initial
    Etat etats[MAX_ETATS];
    int nb_etats = 1;
    
    etats[0].nb_faits = nb_faits;
    etats[0].parent = -1;
    etats[0].regle_appliquee = -1;
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etats[0].faits[i], faits_initiaux[i]);
    }
    
    printf("\nDEBUG: Début de la recherche avec backtracking...\n");
    
    // Mesure du temps d'exécution
    clock_t debut = clock();
    
    // Recherche d'une solution avec backtracking et débogage
    int solution = rechercheSolutionBacktrackDebug(etats, &nb_etats, regles, nb_regles, buts, nb_buts);
    
    // Calcul du temps d'exécution
    clock_t fin = clock();
    double temps_ms = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    
    // Affichage de la solution détaillée
    if(solution >= 0) {
        afficherSolutionDetaillee(etats, regles, solution);
    } else {
        printf("\nDEBUG: Aucune solution trouvée!\n");
    }
    
    printf("\n+----------STATISTIQUES----------+\n");
    printf("| Nombre d'états explorés: %d\n", nb_etats);
    printf("| Temps d'exécution: %.2f ms\n", temps_ms);
    printf("+--------------------------------+\n");
}

/**
 * Fonction pour utiliser le fichier par défaut
 */
void choixFichierParDefaut() {
    const char *fichierDefaut = "assets/monkey.txt";
    analyseFichierDebug(fichierDefaut);
}

/**
 * Fonction pour choisir un fichier personnalisé
 */
void choixFichierUtilisateur() {
    char nomFichier[MAX_CHEMIN];
    
    printf("Entrez le nom du fichier à utiliser : ");
    fgets(nomFichier, MAX_CHEMIN, stdin);
    nomFichier[strcspn(nomFichier, "\r\n")] = 0;
    
    char cheminComplet[MAX_CHEMIN * 2];
    snprintf(cheminComplet, sizeof(cheminComplet), "assets/%s", nomFichier);
    
    FILE *file = fopen(cheminComplet, "r");
    if(file == NULL) {
        printf("Erreur : Le fichier %s n'existe pas ou n'est pas accessible.\n", cheminComplet);
        return;
    }
    fclose(file);
    
    analyseFichierDebug(cheminComplet);
}

int main() {
    afficherEnteteASCII();
    
    int choix = -1;
    while(choix != 0) {
        choix = afficherMenu();
        
        switch(choix) {
            case 0:
                printf("Au revoir !\n");
                break;
            case 1:
                choixFichierParDefaut();
                break;
            case 2:
                choixFichierUtilisateur();
                break;
            default:
                printf("Choix invalide.\n");
        }
        
        if(choix != 0) {
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
        }
    }
    
    return 0;
}