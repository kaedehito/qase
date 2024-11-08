#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>


void display_message(char *text){
	printf("\033[s");
	printf("\033[2A\033[2K");
	printf("%s\n", text);
	printf("\033[u");

}

int get_save_location(char input[100]){
    char cwd[PATH_MAX];
    printf("\033[s");
    if (getcwd(cwd, sizeof(cwd)) == NULL){
         return 1;
    }
    input = "playlist.qpl";
    printf("Enter the save location(default: %s/playlist.qpl): ", cwd);
    int result = scanf("%s", input);
	printf("\033[u");
    return result;
}
