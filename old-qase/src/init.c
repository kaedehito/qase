#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <termio.h>
#include <unistd.h>

#ifndef _INIT_C
#define _INIT_C

// defined at init.c.
// WARN: global variable
Mix_Music *music = NULL;

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int Play(const char *file, bool loop){
        music = Mix_LoadMUS(file);
        if (!music) {
            printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }

        Mix_PlayMusic(music, loop ? -1 : 1);
	return 0;
}

bool init(){
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            disableRawMode();
            return true;
        }
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            disableRawMode();
            return true;
        }
        int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if ((IMG_Init(img_flags) & img_flags) != img_flags) {
            fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
            SDL_Quit();
            return true;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }
	return false;
}
#endif
