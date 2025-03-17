#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIGNE 256
#define MAX_MOTS 50
#define MAX_REGLES 100

typedef char string[50];

/**
 * Structure représentant une règle du système GPS
 * action: nom de l'action
 * preconds: tableau des préconditions nécessaires
 * adds: tableau des éléments à ajouter
 * deletes: tableau des éléments à supprimer
 * nb_preconds, nb_adds, nb_deletes: nombre d'éléments dans chaque tableau
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
 * Analyse une ligne de texte au format "mot:mot1,mot2,..."
 * @param source La chaîne source à analyser
 * @param cible Le tableau qui contiendra les mots extraits
 * @return Le nombre de mots extraits
 */
int parseLine(char source[], string cible[]) {
    printf("Début de la fonction parseLine\n");
    int i = 0, n = 0;
    while(source[i] != ':') i++;
    i++;
    int j = i;
    while(source[i] != '\n' && source[i] != '\0') {
        if(source[i] == ',') {
            memcpy(&cible[n], &source[j], i-j);
            cible[n][i-j] = '\0';
            n++;
            j = i + 1;
        }
        i++;
    }
    memcpy(&cible[n], &source[j], i-j);
    cible[n][i-j] = '\0';
    return n + 1;
}

/**
 * Charge les règles depuis un fichier
 * @param filename Le nom du fichier à lire
 * @param regles Le tableau qui contiendra les règles
 * @return Le nombre de règles chargées
 */
int chargerRegles(const char* filename, Regle regles[]) {
    printf("Début du chargement des règles\n");
    FILE* fichier = fopen(filename, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s\n", filename);
        return -1;
    }

    char ligne[MAX_LIGNE];
    int nb_regles = 0;
    string mots[MAX_MOTS];
    
    while (fgets(ligne, MAX_LIGNE, fichier) != NULL) {
        if (strncmp(ligne, "****", 4) == 0) continue;
        
        if (strstr(ligne, "action:") != NULL) {
            parseLine(ligne, mots);
            strcpy(regles[nb_regles].action, mots[0]);
        }
        else if (strstr(ligne, "preconds:") != NULL) {
            regles[nb_regles].nb_preconds = parseLine(ligne, regles[nb_regles].preconds);
        }
        else if (strstr(ligne, "add:") != NULL) {
            regles[nb_regles].nb_adds = parseLine(ligne, regles[nb_regles].adds);
        }
        else if (strstr(ligne, "delete:") != NULL) {
            regles[nb_regles].nb_deletes = parseLine(ligne, regles[nb_regles].deletes);
            nb_regles++;
        }
    }

    fclose(fichier);
    return nb_regles;
}

/**
 * Charge les faits initiaux ou les buts depuis un fichier
 * @param filename Le nom du fichier à lire
 * @param faits Le tableau qui contiendra les faits
 * @return Le nombre de faits chargés
 */
int chargerFaits(const char* filename, string faits[]) {
    printf("Début du chargement des faits\n");
    FILE* fichier = fopen(filename, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s\n", filename);
        return -1;
    }

    char ligne[MAX_LIGNE];
    int nb_faits = 0;

    while (fgets(ligne, MAX_LIGNE, fichier) != NULL) {
        if (ligne[0] != '*') {
            ligne[strcspn(ligne, "\n")] = 0;
            strcpy(faits[nb_faits++], ligne);
        }
    }

    fclose(fichier);
    return nb_faits;
}

int main() {
    Regle regles[MAX_REGLES];
    string faits_initiaux[MAX_MOTS];
    string buts[MAX_MOTS];

    int nb_regles = chargerRegles("assets/regles.txt", regles);
    int nb_faits = chargerFaits("assets/faits.txt", faits_initiaux);
    int nb_buts = chargerFaits("assets/buts.txt", buts);

    printf("Chargement terminé:\n");
    printf("- %d règles chargées\n", nb_regles);
    printf("- %d faits initiaux chargés\n", nb_faits);
    printf("- %d buts chargés\n", nb_buts);

    return 0;
}
