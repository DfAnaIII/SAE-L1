#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 1000

void process_line(const char* line, int line_number) {
    // Analyse de la ligne et génération des actions
    printf("Action %d: %s\n", line_number, line);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <content>\n", argv[0]);
        return 1;
    }

    char* content = argv[1];
    char* line = strtok(content, "\n");
    int line_number = 1;

    while (line != NULL) {
        process_line(line, line_number++);
        line = strtok(NULL, "\n");
    }

    return 0;
} 