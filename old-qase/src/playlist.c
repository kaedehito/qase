#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INITIAL_CAPACITY 10
#define MAX_LINE_LENGTH 256


char **read_playlist(const char *filename, size_t *line_count){
	FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("\x1b[31mFailed to open file!\x1b[0m");
        return NULL;
    }

    char **lines = malloc(sizeof(char*) * INITIAL_CAPACITY);
    *line_count = 0;
    size_t capacity = INITIAL_CAPACITY;

    char buffer[MAX_LINE_LENGTH];

    while (fgets(buffer, sizeof(buffer), file)) {
        if (*line_count >= capacity) {
            capacity *= 2;
            lines = realloc(lines, sizeof(char*) * capacity);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        lines[*line_count] = malloc(strlen(buffer) + 1);
        strcpy(lines[*line_count], buffer);
        (*line_count)++;
    }

    fclose(file);
    return lines;

}

void freeLines(char **lines, size_t line_count) {
    for (size_t i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
}


int add_playlist(char *file, char *add_music_file){
	FILE *fp;
	fp = fopen(file, "a");
	if(fp == NULL){
		return 1;
	}
	fprintf(fp, "%s\n", add_music_file);
	fclose(fp);
	return 0;
}


