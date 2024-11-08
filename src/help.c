#include <stdio.h>
#include <stdbool.h>
#include "help_message.c"

int ghelp = 0;

void help_display_setup() {
    printf("\x1b[?1049h");
    printf("\x1b[H");
}

void help_home() {
    printf("\x1b[2J\x1b[H");
    printf("------------- Welcome to the Help Section ------------\t\t\t\t[HOME]\n");
    printf("You can press the 'j' key to go back and the 'k' key to move to the next page.\n");
    printf("This help information is also available by running qase with the --buffer-help option.\n");
    printf("\n\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~ INFORMATION ~~~~~~~~~~~~~~~~~~~\n");
    printf("\t### Welcome to qase 3.0! ###\n");
    printf("Many new features have been added in qase 3.0, including:\n");
    printf("    * A new --new-buffer option\n");
    printf("    * A volume adjustment function\n");
    printf("\n");
    printf("...and this is just the beginning!\n");
}


void update_help_page(bool next) {
    if (next) {
        ghelp++;
        if (ghelp > 2) {
            ghelp = 1;
        }
    }
    else {
        ghelp--;
        if (ghelp < 1) {
            ghelp = 0;
        }
    }
}

void show_help() {
    switch (ghelp) {
        case 0:
            help_home();
            break;
        case 1:
            help_1();
            break;
        case 2:
            help_2();
            break;
        default:
            help_home();
            break;
    }
}

void help_display() {
    help_display_setup();
    help_home();

    while (true) {
        char input = getchar();
        
        switch (input) {
            case 'j':
                update_help_page(false);
                show_help();
                break;
            case 'k':
                update_help_page(true);
                show_help();
                break;
            case 'q':
                printf("\x1b[?1049l");
                return;
            default:
                printf("Invalid input. Please press 'j', 'k', or 'q'.\n");
                break;
        }
    }
}

