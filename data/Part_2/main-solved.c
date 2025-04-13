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
} Regle;

/**
 * Structure représentant un état du système
 */
typedef struct {
    string faits[MAX_MOTS];
    int nb_faits;
    int regle_appliquee;  // Indice de la règle appliquée pour atteindre cet état
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
 * Compare deux états pour vérifier s'ils sont identiques
 */
int etatsIdentiques(Etat *etat1, Etat *etat2) {
    if(etat1->nb_faits != etat2->nb_faits) return 0;
    
    for(int i = 0; i < etat1->nb_faits; i++) {
        int trouve = 0;
        for(int j = 0; j < etat2->nb_faits; j++) {
            if(strcmp(etat1->faits[i], etat2->faits[j]) == 0) {
                trouve = 1;
                break;
            }
        }
        if(!trouve) return 0;
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
 * Recherche une solution en utilisant une recherche en largeur (BFS)
 * qui explore systématiquement toutes les possibilités
 */
int rechercheSolution(Etat etats[], int *nb_etats, string buts[], int nb_buts, 
                    Regle regles[], int nb_regles, int *nb_regles_appliquees) {
    // File d'attente pour la recherche en largeur
    int file[MAX_ETATS];
    int debut_file = 0;
    int fin_file = 0;
    
    // Ajouter l'état initial à la file
    file[fin_file++] = 0;
    
    *nb_regles_appliquees = 0;
    
    while(debut_file < fin_file) {
        int indice_courant = file[debut_file++];
        
        // Vérifier si l'état courant atteint les buts
        if(butsAtteints(&etats[indice_courant], buts, nb_buts)) {
            // Calculer le nombre de règles appliquées en remontant le chemin
            int indice = indice_courant;
            *nb_regles_appliquees = 0;
            while(indice > 0) {
                (*nb_regles_appliquees)++;
                indice = etats[indice].parent;
            }
            return indice_courant;
        }
        
        // Pour chaque règle, vérifier si elle est applicable
        for(int i = 0; i < nb_regles; i++) {
            if(preconditionsSatisfaites(&regles[i], &etats[indice_courant])) {
                // Créer un nouvel état en appliquant la règle
                Etat nouvel_etat;
                appliquerRegle(&regles[i], &etats[indice_courant], &nouvel_etat);
                nouvel_etat.regle_appliquee = i;
                nouvel_etat.parent = indice_courant;
                
                // Vérifier si cet état existe déjà
                int etat_deja_vu = 0;
                for(int j = 0; j < *nb_etats; j++) {
                    if(etatsIdentiques(&nouvel_etat, &etats[j])) {
                        etat_deja_vu = 1;
                        break;
                    }
                }
                
                // Si l'état est nouveau et qu'on n'a pas atteint la limite
                if(!etat_deja_vu && *nb_etats < MAX_ETATS) {
                    etats[*nb_etats] = nouvel_etat;
                    file[fin_file++] = *nb_etats;
                    (*nb_etats)++;
                }
            }
        }
    }
    
    return -1; // Aucune solution trouvée
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
    
    printf("\n+------------------PLAN TROUVE------------------+\n|\n");
    printf("| Solution trouvée (%d étapes):\n", longueur);
    for(int i = longueur - 1; i >= 0; i--) {
        int regle = etats[chemin[i]].regle_appliquee;
        printf("| - %s\n", regles[regle].action);
    }
    printf("|\n+------------------FIN DU PLAN------------------+\n\n");
}

/**
 * Fonction pour afficher l'entête ASCII
 */
void afficherEnteteASCII() {
    printf("+----------------------------------------------+\n");
    printf("|                                              |\n");
    printf("|  GPS avec Chaînage Avant Amélioré (Part 2)   |\n");
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
    Etat etats[MAX_ETATS];
    etats[0].nb_faits = nb_faits;
    etats[0].parent = -1;
    etats[0].regle_appliquee = -1;
    
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etats[0].faits[i], faits_initiaux[i]);
    }
    afficherEtat(&etats[0]);
    printf("\n");
    
    // Affichage des buts
    printf("\nButs à atteindre: ");
    printf("[ ");
    for(int i = 0; i < nb_buts; i++) {
        printf("%s", buts[i]);
        if(i < nb_buts - 1) printf(", ");
    }
    printf(" ]\n");
    
    printf("\nRecherche d'une solution...\n");
    
    // Mesure du temps d'exécution
    clock_t debut = clock();
    
    // Nombre d'états générés
    int nb_etats = 1; // État initial déjà compté
    
    // Recherche d'une solution avec BFS
    int nb_regles_appliquees = 0;
    int solution = rechercheSolution(etats, &nb_etats, buts, nb_buts, regles, nb_regles, &nb_regles_appliquees);
    
    // Calcul du temps d'exécution
    clock_t fin = clock();
    double temps_ms = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    
    printf("\nRésultat final:\n");
    
    if(solution >= 0) {
        printf("✅ Solution trouvée!\n");
        printf("État final: ");
        afficherEtat(&etats[solution]);
        printf("\n");
        
        // Afficher le plan (séquence d'actions)
        afficherSolution(etats, regles, solution);
    } else {
        printf("❌ Aucune solution trouvée\n");
    }
    
    printf("\nStatistiques:\n");
    printf("- Nombre d'états explorés: %d\n", nb_etats);
    printf("- Nombre de règles appliquées dans la solution: %d\n", nb_regles_appliquees);
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
    
    printf("Ce programme implémente un moteur de raisonnement en chaînage avant amélioré\n");
    printf("capable de résoudre une plus grande variété de problèmes grâce à la recherche en largeur.\n\n");
    
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