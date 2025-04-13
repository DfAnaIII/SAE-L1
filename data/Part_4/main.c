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
 * Recherche avec backtracking pour trouver une solution
 */
int rechercheSolutionBacktrack(Etat etats[], int *nb_etats, Regle regles[], int nb_regles, 
                             string buts[], int nb_buts) {
    int possible = 1;
    int IR = 0;  // Indice à partir duquel on cherche une règle applicable
    int etat_courant = 0;
    int max_profondeur = 100; // Limite de profondeur pour éviter les boucles infinies
    int profondeur_actuelle = 0;
    
    while(possible && !butsAtteints(&etats[etat_courant], buts, nb_buts) && profondeur_actuelle < max_profondeur) {
        int regle_trouvee = 0;
        
        // Sécurité : vérifier si on a atteint la limite du tableau des états
        if(*nb_etats >= MAX_ETATS - 1) {
            printf("\nATTENTION: Limite du nombre d'états atteinte (%d). Arrêt de la recherche.\n", MAX_ETATS);
            return -1;
        }
        
        // Cherche une règle applicable d'indice > IR
        for(int i = IR; i < nb_regles; i++) {
            if(preconditionsSatisfaites(&regles[i], &etats[etat_courant])) {
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
                
                if(!etat_deja_visite) {
                    etat_courant = *nb_etats;
                    (*nb_etats)++;
                    profondeur_actuelle++;
                    IR = 0;  // Réinitialise IR pour le nouvel état
                    regle_trouvee = 1;
                    break;
                }
            }
        }
        
        if(!regle_trouvee) {
            if(etat_courant > 0) {
                // Backtrack: revient à l'état précédent
                IR = etats[etat_courant].regle_appliquee + 1;
                etat_courant = etats[etat_courant].parent;
                profondeur_actuelle--;
                // Ne pas décrémenter nb_etats car on veut garder trace de tous les états explorés
            } else {
                possible = 0;  // Aucune solution trouvée
            }
        }
    }
    
    if(profondeur_actuelle >= max_profondeur) {
        printf("\nATTENTION: Profondeur maximale atteinte (%d). La recherche a été arrêtée.\n", max_profondeur);
        return -1;
    }
    
    return possible ? etat_courant : -1;
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
    printf("|         GPS avec Backtracking (Part 3)       |\n");
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
    printf("|  3) Créer un nouveau fichier                      |\n");
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
 * Fonction pour demander un texte à l'utilisateur
 */
void demanderTexte(const char *question, char *buffer, int bufferSize) {
    printf("%s", question);
    fgets(buffer, bufferSize, stdin);
    // Enlever le retour à la ligne
    buffer[strcspn(buffer, "\r\n")] = 0;
}

/**
 * Fonction pour créer un nouveau fichier au format approprié
 */
int creerFichier() {
    char nomFichier[MAX_CHEMIN];
    
    printf("Entrez le nom du fichier à créer (ex: monFichier.txt) : ");
    fgets(nomFichier, MAX_CHEMIN, stdin);
    nomFichier[strcspn(nomFichier, "\r\n")] = 0;
    
    char cheminComplet[MAX_CHEMIN * 2];
    snprintf(cheminComplet, sizeof(cheminComplet), "assets/%s", nomFichier);
    
    FILE *fp = fopen(cheminComplet, "w");
    if(!fp) {
        printf("Impossible de créer le fichier '%s'.\n", cheminComplet);
        return 0;
    }
    
    char buf[MAX_LIGNE];
    demanderTexte("Entrez les conditions initiales (séparées par des virgules):\n> ", buf, MAX_LIGNE);
    fprintf(fp, "start:%s\n", buf);
    
    demanderTexte("Entrez les conditions d'arrivée (finish), séparées par des virgules:\n> ", buf, MAX_LIGNE);
    fprintf(fp, "finish:%s\n", buf);
    
    int nbActions = 0;
    printf("Combien d'actions voulez-vous saisir ? ");
    scanf("%d", &nbActions);
    getchar(); // Pour capturer le retour à la ligne
    
    for(int i = 0; i < nbActions; i++) {
        fprintf(fp, "****\n");
        
        // Action (nom)
        demanderTexte("\nNom de l'action : ", buf, MAX_LIGNE);
        fprintf(fp, "action:%s\n", buf);
        
        // Préconditions
        demanderTexte("Préconditions (séparées par des virgules) : ", buf, MAX_LIGNE);
        fprintf(fp, "preconds:%s\n", buf);
        
        // Faits ajoutés
        demanderTexte("Faits ajoutés (séparés par des virgules) : ", buf, MAX_LIGNE);
        fprintf(fp, "add:%s\n", buf);
        
        // Faits supprimés
        demanderTexte("Faits supprimés (séparés par des virgules) : ", buf, MAX_LIGNE);
        fprintf(fp, "delete:%s\n", buf);
    }
    
    fclose(fp);
    printf("\nFichier '%s' créé avec succès dans le dossier 'assets'.\n", nomFichier);
    return 1;
}

/**
 * Fonction pour analyser et résoudre un problème depuis un fichier
 */
void analyseFichier(const char *nomFichier) {
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];
    Regle regles[MAX_REGLES];
    int nb_faits = 0, nb_buts = 0, nb_regles = 0;
    
    if(!chargerFichierFormat(nomFichier, faits_initiaux, &nb_faits, buts, &nb_buts, regles, &nb_regles)) {
        return; // Erreur déjà affichée
    }
    
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
    
    // Mesure du temps d'exécution
    clock_t debut = clock();
    
    // Recherche d'une solution avec backtracking
    int solution = rechercheSolutionBacktrack(etats, &nb_etats, regles, nb_regles, buts, nb_buts);
    
    // Calcul du temps d'exécution
    clock_t fin = clock();
    double temps_ms = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    
    // Affichage de la solution
    afficherSolution(etats, regles, solution);
    
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
            case 3:
                if(creerFichier()) {
                    printf("Voulez-vous tester ce fichier maintenant ? (o/n) ");
                    char reponse[2];
                    fgets(reponse, 2, stdin);
                    viderBuffer();
                    
                    if(reponse[0] == 'o' || reponse[0] == 'O') {
                        // Demander le nom
                        char nomFichier[MAX_CHEMIN];
                        printf("Entrez à nouveau le nom du fichier : ");
                        fgets(nomFichier, MAX_CHEMIN, stdin);
                        nomFichier[strcspn(nomFichier, "\r\n")] = 0;
                        
                        char cheminComplet[MAX_CHEMIN * 2];
                        snprintf(cheminComplet, sizeof(cheminComplet), "assets/%s", nomFichier);
                        analyseFichier(cheminComplet);
                    }
                }
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