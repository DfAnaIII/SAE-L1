#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_terminable(const char* content) {
    // Implémentez ici votre logique de vérification
    // Pour l'exemple, on vérifie simplement si le contenu n'est pas vide
    return strlen(content) > 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Something went wrong\n");
        return 1;
    }

    if (is_terminable(argv[1])) {
        printf("Correct\n");
    } else {
        printf("Incorrect\n");
    }

    return 0;
} 