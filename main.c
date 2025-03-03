//
// Created by starf on 27/01/2025.
//

//Version : 1.0

#include <stdlib.h>
#include <stdio.h>

// Pensez aux conditions \n !


typedef struct Global {
    char start[100];
    char finish[100];
};

// Fonction pour afficher le contenue du ficher
void Affiche(const char* nomFichier) {

    char c;

    FILE* fichier = fopen(nomFichier, "r");

    if(fichier==NULL){
        printf("Le fichier de depart est null");
        return;
    }

    while ((c = fgetc(fichier)) != EOF) {
        putchar(c);
    }

    fclose(fichier);
}

void Struct_Dyna(int NB_ACTION) {
    int i;
    for (i = 0; i < NB_ACTION; i++) {
        struct i {

            char action [100];
            char preconds [100];
            char add [100];
            char delete[100];
        };
    }
}

int main() {
    const char* nomFichier = "monkeys.txt";
    Affiche(nomFichier);

    return 0;
}

