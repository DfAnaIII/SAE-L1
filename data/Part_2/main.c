#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LIGNE 256
#define MAX_MOTS 50
#define MAX_REGLES 100
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
} Regle;

/**
 * Structure représentant un état du système
 */
typedef struct {
    string faits[MAX_MOTS];
    int nb_faits;
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
        if(!contientFait(etat, regle->preconds[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * Affiche les faits d'un état 
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
 * Applique une règle à un état pour le modifier
 */
void appliquerRegle(Regle *regle, Etat *etat) {
    // Suppressions
    for(int i = 0; i < regle->nb_deletes; i++) {
        for(int j = 0; j < etat->nb_faits; j++) {
            if(strcmp(etat->faits[j], regle->deletes[i]) == 0) {
                // Supprimer le fait j en décalant les suivants
                for(int k = j; k < etat->nb_faits - 1; k++) {
                    strcpy(etat->faits[k], etat->faits[k+1]);
                }
                etat->nb_faits--;
                j--; // Pour revérifier à la même position
                break;
            }
        }
    }
    
    // Ajouts
    for(int i = 0; i < regle->nb_adds; i++) {
        if(!contientFait(etat, regle->adds[i])) {
            strcpy(etat->faits[etat->nb_faits++], regle->adds[i]);
        }
    }
}

/**
 * Vérifie si tous les buts sont atteints dans un état
 */
int butsAtteints(Etat *etat, string buts[], int nb_buts) {
    for(int i = 0; i < nb_buts; i++) {
        if(!contientFait(etat, buts[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * Recherche une solution en chaînage avant simple
 * Retourne 1 si une solution est trouvée, 0 sinon
 */
int rechercheSolution(Etat *etat, string buts[], int nb_buts, Regle regles[], int nb_regles, int *nb_regles_appliquees) {
    *nb_regles_appliquees = 0;
    
    // Tant que le but n'est pas atteint
    while(!butsAtteints(etat, buts, nb_buts)) {
        int regle_trouvee = 0;
        
        // Chercher une règle applicable
        for(int i = 0; i < nb_regles; i++) {
            if(preconditionsSatisfaites(&regles[i], etat)) {
                // Appliquer la règle
                printf("Application de la règle: %s\n", regles[i].action);
                appliquerRegle(&regles[i], etat);
                (*nb_regles_appliquees)++;
                regle_trouvee = 1;
                break;
            }
        }
        
        // Si aucune règle n'est applicable, échec
        if(!regle_trouvee) {
            return 0;
        }
        
        // Vérifier si on a dépassé un nombre maximal d'itérations
        if(*nb_regles_appliquees > 100) {
            printf("Alerte: Nombre maximal d'itérations atteint (100). Arrêt de la recherche.\n");
            return 0;
        }
    }
    
    return 1; // But atteint
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
    printf("|      GPS avec Chaînage Avant (Partie 2)      |\n");
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
 * Fonction pour analyser et résoudre un problème depuis un fichier
 */
void analyseFichier(const char *nomFichier) {
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Regle regles[MAX_REGLES];
    int nb_faits = 0, nb_buts = 0, nb_regles = 0;
    
    printf("\nChargement du fichier '%s'...\n", nomFichier);
    
    if(!chargerFichierFormat(nomFichier, faits_initiaux, &nb_faits, buts, &nb_buts, regles, &nb_regles)) {
        return; // Erreur déjà affichée
    }
    
    printf("Fichier chargé avec succès!\n");
    
    // Affichage des faits initiaux
    printf("\nÉtat initial: ");
    Etat etat_initial;
    etat_initial.nb_faits = nb_faits;
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etat_initial.faits[i], faits_initiaux[i]);
    }
    afficherEtat(&etat_initial);
    printf("\n");
    
    // Affichage des buts
    printf("\nButs à atteindre: ");
    printf("[ ");
    for(int i = 0; i < nb_buts; i++) {
        printf("%s", buts[i]);
        if(i < nb_buts - 1) printf(", ");
    }
    printf(" ]\n");
    
    // Initialisation de l'état courant
    Etat etat_courant;
    etat_courant.nb_faits = nb_faits;
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etat_courant.faits[i], faits_initiaux[i]);
    }
    
    printf("\nRecherche d'une solution...\n");
    
    // Mesure du temps d'exécution
    clock_t debut = clock();
    
    // Recherche d'une solution en chaînage avant simple
    int nb_regles_appliquees = 0;
    int solution = rechercheSolution(&etat_courant, buts, nb_buts, regles, nb_regles, &nb_regles_appliquees);
    
    // Calcul du temps d'exécution
    clock_t fin = clock();
    double temps_ms = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    
    printf("\nRésultat final:\n");
    
    if(solution) {
        printf("✅ Solution trouvée!\n");
        printf("État final: ");
        afficherEtat(&etat_courant);
        printf("\n");
    } else {
        printf("❌ Aucune solution trouvée\n");
    }
    
    printf("\nStatistiques:\n");
    printf("- Nombre de règles appliquées: %d\n", nb_regles_appliquees);
    printf("- Temps d'exécution: %.2f ms\n", temps_ms);
}

/**
 * Fonction pour utiliser le fichier par défaut
 */
void choixFichierParDefaut() {
    const char *fichierDefaut = "assets/monkey.txt";
    analyseFichier(fichierDefaut);
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
    
    analyseFichier(cheminComplet);
}

int main() {
    afficherEnteteASCII();
    
    printf("Ce programme implémente un moteur de raisonnement en chaînage avant simple.\n\n");
    
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
