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
 * Vérifie si un fait existe dans un état - avec debug
 */
int contientFaitDebug(Etat *etat, const char *fait) {
    printf("  🔍 Recherche du fait '%s' dans l'état courant...\n", fait);
    
    for(int i = 0; i < etat->nb_faits; i++) {
        if(strcmp(etat->faits[i], fait) == 0) {
            printf("      ✅ Le fait '%s' est présent dans l'état.\n", fait);
            return 1;
        }
    }
    
    printf("      ❌ Le fait '%s' est absent de l'état.\n", fait);
    return 0;
}

/**
 * Vérifie si toutes les préconditions d'une règle sont satisfaites dans un état - avec debug
 */
int preconditionsSatisfaitesDebug(Regle *regle, Etat *etat) {
    printf("\n📋 Vérification des préconditions pour la règle '%s':\n", regle->action);
    
    if(regle->nb_preconds == 0) {
        printf("   → Cette règle n'a pas de préconditions (toujours applicable).\n");
        return 1;
    }
    
    for(int i = 0; i < regle->nb_preconds; i++) {
        printf("   → Précondition %d/%d: '%s'\n", i+1, regle->nb_preconds, regle->preconds[i]);
        if(!contientFaitDebug(etat, regle->preconds[i])) {
            printf("   ❌ Les préconditions ne sont PAS toutes satisfaites pour la règle '%s'.\n", regle->action);
            return 0;
        }
    }
    
    printf("   ✅ Toutes les préconditions sont satisfaites pour la règle '%s'.\n", regle->action);
    return 1;
}

/**
 * Affiche les faits d'un état avec formatage détaillé
 */
void afficherEtatDebug(Etat *etat, const char *titre) {
    printf("\n🔹 %s: [ ", titre);
    for(int i = 0; i < etat->nb_faits; i++) {
        printf("%s", etat->faits[i]);
        if(i < etat->nb_faits - 1) printf(", ");
    }
    printf(" ]\n");
}

/**
 * Applique une règle à un état pour le modifier - avec debug détaillé
 */
void appliquerRegleDebug(Regle *regle, Etat *etat) {
    printf("\n⚙️ APPLICATION DE LA RÈGLE: '%s'\n", regle->action);
    
    // Affichage de l'état avant modification
    afficherEtatDebug(etat, "État AVANT application");
    
    // Suppressions
    if(regle->nb_deletes > 0) {
        printf("\n  🗑️ Faits à supprimer (%d):\n", regle->nb_deletes);
        for(int i = 0; i < regle->nb_deletes; i++) {
            printf("     - '%s'", regle->deletes[i]);
            
            int supprime = 0;
            for(int j = 0; j < etat->nb_faits; j++) {
                if(strcmp(etat->faits[j], regle->deletes[i]) == 0) {
                    // Supprimer le fait j en décalant les suivants
                    printf(" ✓ (supprimé)");
                    for(int k = j; k < etat->nb_faits - 1; k++) {
                        strcpy(etat->faits[k], etat->faits[k+1]);
                    }
                    etat->nb_faits--;
                    j--; // Pour revérifier à la même position
                    supprime = 1;
                    break;
                }
            }
            
            if(!supprime) {
                printf(" ⚠️ (non présent, aucune action)");
            }
            
            printf("\n");
        }
    } else {
        printf("\n  🗑️ Aucun fait à supprimer.\n");
    }
    
    // Ajouts
    if(regle->nb_adds > 0) {
        printf("\n  ➕ Faits à ajouter (%d):\n", regle->nb_adds);
        for(int i = 0; i < regle->nb_adds; i++) {
            printf("     - '%s'", regle->adds[i]);
            
            if(!contientFaitDebug(etat, regle->adds[i])) {
                strcpy(etat->faits[etat->nb_faits++], regle->adds[i]);
                printf(" ✓ (ajouté)\n");
            } else {
                printf(" ⚠️ (déjà présent, non ajouté)\n");
            }
        }
    } else {
        printf("\n  ➕ Aucun fait à ajouter.\n");
    }
    
    // Affichage de l'état après modification
    afficherEtatDebug(etat, "État APRÈS application");
}

/**
 * Vérifie si tous les buts sont atteints dans un état - avec debug
 */
int butsAtteintsDebug(Etat *etat, string buts[], int nb_buts) {
    printf("\n🎯 Vérification des buts:\n");
    
    for(int i = 0; i < nb_buts; i++) {
        printf("   → But %d/%d: '%s'\n", i+1, nb_buts, buts[i]);
        if(!contientFaitDebug(etat, buts[i])) {
            printf("   ❌ Les buts ne sont PAS tous atteints.\n");
            return 0;
        }
    }
    
    printf("   ✅ TOUS LES BUTS SONT ATTEINTS! 🎉\n");
    return 1;
}

/**
 * Recherche une solution en chaînage avant simple - avec debug détaillé
 * Retourne 1 si une solution est trouvée, 0 sinon
 */
int rechercheSolutionDebug(Etat *etat, string buts[], int nb_buts, Regle regles[], int nb_regles, int *nb_regles_appliquees) {
    *nb_regles_appliquees = 0;
    
    printf("\n🔄 DÉBUT DE LA RECHERCHE EN CHAÎNAGE AVANT\n");
    printf("===========================================\n");
    
    // Tant que le but n'est pas atteint
    int iteration = 1;
    while(!butsAtteintsDebug(etat, buts, nb_buts)) {
        printf("\n📌 ITÉRATION %d\n", iteration++);
        printf("-----------------\n");
        
        int regle_trouvee = 0;
        
        // Chercher une règle applicable
        for(int i = 0; i < nb_regles; i++) {
            printf("\n👉 Examen de la règle %d: '%s'\n", i+1, regles[i].action);
            
            if(preconditionsSatisfaitesDebug(&regles[i], etat)) {
                // Appliquer la règle
                printf("\n✨ La règle '%s' est applicable! Application...\n", regles[i].action);
                appliquerRegleDebug(&regles[i], etat);
                (*nb_regles_appliquees)++;
                regle_trouvee = 1;
                break;
            } else {
                printf("   ⏭️ La règle '%s' n'est pas applicable, passage à la suivante...\n", regles[i].action);
            }
        }
        
        // Si aucune règle n'est applicable, échec
        if(!regle_trouvee) {
            printf("\n❌ ÉCHEC: Aucune règle applicable pour faire progresser la recherche.\n");
            return 0;
        }
        
        // Vérifier si on a dépassé un nombre maximal d'itérations
        if(*nb_regles_appliquees > 100) {
            printf("\n⚠️ ALERTE: Nombre maximal d'itérations atteint (100).\n");
            printf("   Cela peut indiquer une boucle infinie dans l'application des règles.\n");
            return 0;
        }
    }
    
    printf("\n✅ SUCCÈS! Une solution a été trouvée après %d applications de règles.\n", *nb_regles_appliquees);
    return 1; // But atteint
}

/**
 * Fonction pour diviser une chaîne en utilisant le délimiteur et supprimer les espaces
 * Avec affichage de debug
 */
void diviserChaineDebug(const char *chaine, char delim, string resultat[], int *nb_elements) {
    *nb_elements = 0;
    char buffer[MAX_LIGNE];
    strcpy(buffer, chaine);
    
    printf("  🔪 Division de la chaîne: \"%s\" avec délimiteur '%c'\n", chaine, delim);
    
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
            printf("     → Élément %d: \"%s\"\n", *nb_elements, debut);
        }
        
        token = strtok(NULL, &delim);
    }
    
    printf("  ✂️ %d éléments extraits.\n", *nb_elements);
}

/**
 * Fonction pour charger un fichier au format spécifié - avec debug détaillé
 */
int chargerFichierFormatDebug(const char *fichier, string faits_initiaux[], int *nb_faits, 
                             string buts[], int *nb_buts, Regle regles[], int *nb_regles) {
    FILE *f = fopen(fichier, "r");
    if(!f) {
        printf("❌ ERREUR: Impossible d'ouvrir le fichier %s\n", fichier);
        return 0;
    }
    
    printf("📂 Ouverture du fichier %s réussie.\n", fichier);
    
    char ligne[MAX_LIGNE];
    *nb_regles = 0;
    *nb_faits = 0;
    *nb_buts = 0;
    
    Regle regle_courante;
    int dans_action = 0;
    int num_ligne = 0;
    
    printf("\n📑 ANALYSE DU FICHIER:\n");
    printf("====================\n");
    
    while(fgets(ligne, MAX_LIGNE, f)) {
        num_ligne++;
        // Supprimer le retour à la ligne
        ligne[strcspn(ligne, "\r\n")] = 0;
        
        printf("\n📄 Ligne %d: \"%s\"\n", num_ligne, ligne);
        
        // Ligne vide ou trop courte
        if(strlen(ligne) <= 1) {
            printf("   → Ligne ignorée (vide ou trop courte).\n");
            continue;
        }
        
        // Séparateur d'actions
        if(strncmp(ligne, "****", 4) == 0) {
            printf("   → Séparateur d'actions détecté.\n");
            
            if(dans_action) {
                // Enregistrer la règle précédente
                printf("   📝 Enregistrement de la règle précédente: '%s'\n", regle_courante.action);
                regles[(*nb_regles)++] = regle_courante;
            }
            
            dans_action = 1;
            // Initialiser une nouvelle règle
            printf("   🆕 Initialisation d'une nouvelle règle.\n");
            memset(&regle_courante, 0, sizeof(Regle));
            continue;
        }
        
        // Parser selon le type de ligne
        if(strncmp(ligne, "start:", 6) == 0) {
            printf("   🏁 Détection des faits initiaux:\n");
            diviserChaineDebug(ligne + 6, ',', faits_initiaux, nb_faits);
        }
        else if(strncmp(ligne, "finish:", 7) == 0) {
            printf("   🎯 Détection des buts:\n");
            diviserChaineDebug(ligne + 7, ',', buts, nb_buts);
        }
        else if(dans_action) {
            if(strncmp(ligne, "action:", 7) == 0) {
                printf("   ⚙️ Détection d'une action: '%s'\n", ligne + 7);
                strcpy(regle_courante.action, ligne + 7);
            }
            else if(strncmp(ligne, "preconds:", 9) == 0) {
                printf("   📋 Détection des préconditions:\n");
                diviserChaineDebug(ligne + 9, ',', regle_courante.preconds, &regle_courante.nb_preconds);
            }
            else if(strncmp(ligne, "add:", 4) == 0) {
                printf("   ➕ Détection des ajouts:\n");
                diviserChaineDebug(ligne + 4, ',', regle_courante.adds, &regle_courante.nb_adds);
            }
            else if(strncmp(ligne, "delete:", 7) == 0) {
                printf("   🗑️ Détection des suppressions:\n");
                diviserChaineDebug(ligne + 7, ',', regle_courante.deletes, &regle_courante.nb_deletes);
            }
        }
    }
    
    // Enregistrer la dernière règle si elle existe
    if(dans_action) {
        printf("\n📝 Enregistrement de la dernière règle: '%s'\n", regle_courante.action);
        regles[(*nb_regles)++] = regle_courante;
    }
    
    printf("\n📊 RÉSUMÉ DU CHARGEMENT:\n");
    printf("   - %d faits initiaux\n", *nb_faits);
    printf("   - %d buts\n", *nb_buts);
    printf("   - %d règles\n", *nb_regles);
    
    fclose(f);
    printf("📂 Fermeture du fichier %s.\n", fichier);
    return 1;
}

/**
 * Fonction pour afficher l'entête ASCII
 */
void afficherEnteteASCII() {
    printf("+------------------------------------------------------+\n");
    printf("|                                                      |\n");
    printf("|      GPS avec Chaînage Avant - Mode Debug (P2)       |\n");
    printf("|                                                      |\n");
    printf("+------------------------------------------------------+\n\n");
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
 * Fonction pour analyser et résoudre un problème depuis un fichier - avec debug détaillé
 */
void analyseFichierDebug(const char *nomFichier) {
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Regle regles[MAX_REGLES];
    int nb_faits = 0, nb_buts = 0, nb_regles = 0;
    
    printf("\n🔍 ANALYSE DU FICHIER '%s':\n", nomFichier);
    printf("==========================\n");
    
    if(!chargerFichierFormatDebug(nomFichier, faits_initiaux, &nb_faits, buts, &nb_buts, regles, &nb_regles)) {
        return; // Erreur déjà affichée
    }
    
    printf("\n✅ Fichier chargé avec succès!\n");
    
    // Affichage détaillé des faits initiaux
    printf("\n🏁 ÉTAT INITIAL (%d faits):\n", nb_faits);
    for(int i = 0; i < nb_faits; i++) {
        printf("   - '%s'\n", faits_initiaux[i]);
    }
    
    // Affichage détaillé des buts
    printf("\n🎯 BUTS À ATTEINDRE (%d buts):\n", nb_buts);
    for(int i = 0; i < nb_buts; i++) {
        printf("   - '%s'\n", buts[i]);
    }
    
    // Affichage détaillé des actions/règles
    printf("\n⚙️ RÈGLES DISPONIBLES (%d règles):\n", nb_regles);
    for(int i = 0; i < nb_regles; i++) {
        printf("\n📜 Règle %d: '%s'\n", i+1, regles[i].action);
        
        printf("   📋 Préconditions (%d):\n", regles[i].nb_preconds);
        for(int j = 0; j < regles[i].nb_preconds; j++) {
            printf("      - '%s'\n", regles[i].preconds[j]);
        }
        
        printf("   ➕ Ajouts (%d):\n", regles[i].nb_adds);
        for(int j = 0; j < regles[i].nb_adds; j++) {
            printf("      - '%s'\n", regles[i].adds[j]);
        }
        
        printf("   🗑️ Suppressions (%d):\n", regles[i].nb_deletes);
        for(int j = 0; j < regles[i].nb_deletes; j++) {
            printf("      - '%s'\n", regles[i].deletes[j]);
        }
    }
    
    // Initialisation de l'état courant
    Etat etat_courant;
    etat_courant.nb_faits = nb_faits;
    for(int i = 0; i < nb_faits; i++) {
        strcpy(etat_courant.faits[i], faits_initiaux[i]);
    }
    
    printf("\n🚀 DÉBUT DE LA RECHERCHE AVEC CHAÎNAGE AVANT...\n");
    printf("==============================================\n");
    
    // Mesure du temps d'exécution
    clock_t debut = clock();
    
    // Recherche d'une solution en chaînage avant simple avec debug
    int nb_regles_appliquees = 0;
    int solution = rechercheSolutionDebug(&etat_courant, buts, nb_buts, regles, nb_regles, &nb_regles_appliquees);
    
    // Calcul du temps d'exécution
    clock_t fin = clock();
    double temps_ms = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    
    printf("\n📊 RÉSULTAT FINAL:\n");
    printf("=================\n");
    
    if(solution) {
        printf("\n✅ SOLUTION TROUVÉE!\n");
        afficherEtatDebug(&etat_courant, "État final");
    } else {
        printf("\n❌ AUCUNE SOLUTION TROUVÉE.\n");
        afficherEtatDebug(&etat_courant, "Dernier état atteint");
    }
    
    printf("\n📈 STATISTIQUES:\n");
    printf("   - Nombre de règles appliquées: %d\n", nb_regles_appliquees);
    printf("   - Temps d'exécution: %.2f ms\n", temps_ms);
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
    
    printf("🔍 Vérification de l'existence du fichier '%s'...\n", cheminComplet);
    FILE *file = fopen(cheminComplet, "r");
    if(file == NULL) {
        printf("❌ ERREUR : Le fichier %s n'existe pas ou n'est pas accessible.\n", cheminComplet);
        return;
    }
    fclose(file);
    printf("✅ Le fichier existe et est accessible.\n");
    
    analyseFichierDebug(cheminComplet);
}

int main() {
    afficherEnteteASCII();
    
    printf("🔍 Mode DEBUG activé - Toutes les étapes seront détaillées.\n");
    printf("Ce programme explique en détail le fonctionnement du moteur de raisonnement.\n\n");
    
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
