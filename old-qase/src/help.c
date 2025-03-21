#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include <pthread.h>  // スレッド関連

#ifndef _HELP_C
#define _HELP_C

#include "help_message.c"

volatile bool nextpage = true;
int ghelp = 0;

void help_display_setup() {
    printf("\x1b[?1049h");  // スクリーンバッファ切替（仮想スクリーン）
    printf("\x1b[H");  // カーソルをホームに
}

void help_home() {
        printf("\x1b[2J\x1b[H");
        printf("\x1b[33m------------- Welcome to the Help Section ------------\x1b[0m\t\t[HOME]\n\n");
        printf("You can press the 'j' key to go back and the 'k' key to move to the next page.\n");
        printf("This help information is also available by running qase with the --buffer-help option.\n");
        printf("\n\n");
        printf("\x1b[38;5;135m================ INFORMATION ================\x1b[0m\n");
        printf("    \x1b[1;38;5;81m### Welcome to qase 3.0! ###\x1b[0m\n");
        printf("Many new features have been added in qase 3.0, including:\n");
        printf("    * A new --new-buffer option\n");
        printf("    * A volume adjustment function\n");
        printf("\n");
        printf("...and this is just the beginning!\n");
        printf("\n");
        printf("    \x1b[1;38;5;81m### qase3.1 is under development! ###\x1b[0m\n");
        printf("Qase3.1 will include minor bug fixes, rpm file distribution, etc.\n");
}


void display_page(int page) {
    // 画面更新
    printf("\x1b[2J\x1b[H");  // 画面クリア
    switch (page) {
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

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // 非エコー化
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        return ch;
    }
    return -1;
}

void update_help_page(bool next) {
    if (next) {
        ghelp++;
        if (ghelp > 2) {
            ghelp = 2;
        }
    } else {
        ghelp--;
        if (ghelp < 1) {
            ghelp = 0;
        }
    }
}

void show_help() {
    display_page(ghelp);
}

void help_display() {
    help_display_setup();
    show_help();
    while (true) {
        char input = kbhit();
        switch (input) {
            case 'j':
                update_help_page(false);  // 前のページに移動
                show_help();
                break;
            case 'k':
                update_help_page(true);  // 次のページに移動
                show_help();
                break;
            case 'q':
                printf("\x1b[?1049l");
                return;
            default:
                // 何もしない
                break;
        }
    }
}

#endif
