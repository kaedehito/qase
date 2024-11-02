/*
 * Copyright (c) 2024 ogasawara futo <pik6cs@gmail.com>
 *
 * This software is licensed under the MIT License.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include <string.h>
#include <id3tag.h>
#include <argp.h>
#include "image.c"
#include "musicname.c"

volatile int stop = 0;
volatile int loop = 0;
int disable_cover = 0;
char *music_file = NULL;

void ver() {
    printf("qase 2.0\n");
    printf("Copyright (c) 2024 futo ogasawara <pik6cs@gmail.com>\n");
    printf("This software is licensed under the MIT License\n");
}


struct arguments {
    char *args[1];
    int loop;
    int disable_cover;
    int version;
};

static struct argp_option options[] = {
    {"loop", 'l', 0, 0, "Loop the music", 0},
    {"no-cover", 'c', 0, 0, "Disable cover image", 0},
    {"version", 'v', 0, 0, "Display qase version", 0},
    {0, 0, 0, 0, NULL, 0} 
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'l':
            arguments->loop = 1;
            break;
        case 'c':
            arguments->disable_cover = 1;
            break;
	case 'v':
	    ver();
	    exit(0);
	    break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1) {
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            // -c フラグがある場合は getCoverImage を実行しない
            if (arguments->disable_cover == 0) {
                getCoverImage(arg);
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, "FILE", "Extremely simple music playback program", 0, NULL, 0};


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

void *clearLine(void *arg) {
    int *remaining_time = (int *)arg;

    while (*remaining_time > 0) {
        if (stop == 1) {
            printf("\033[F\033[K⏸ Paused\n");
            while (stop == 1) {
                sleep(1);
            }
        } else if (stop == 2) {
            printf("\033[F\033[K ⏹ Quit\n");
            break;
        }

        int minutes = *remaining_time / 60;
        int seconds = *remaining_time % 60;
        if (minutes == 0) {
            printf("\033[F\033[K▶ %d seconds\n", seconds);
        } else {
            printf("\033[F\033[K▶ %d minute, %d seconds\n", minutes, seconds);
        }
        fflush(stdout);

        sleep(1);
        (*remaining_time)--;

        if (loop && *remaining_time == 0) {
            *remaining_time = *(int *)arg;
        }
    }

    return NULL;
}



int main(int argc, char *argv[]) {
    struct arguments arguments;
    arguments.loop = 0;
    arguments.disable_cover = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    
    music_file = arguments.args[0]; 
    loop = arguments.loop;
    disable_cover = arguments.disable_cover;

   if (music_file) {
        enableRawMode();

        char text[200];
        snprintf(text, sizeof(text), "ffprobe -i \"%s\" -show_entries format=duration -v quiet -of csv=\"p=0\"", music_file);
        FILE* fp = popen(text, "r");

        if (fp == NULL) {
            printf("Failed to run command\n");
            disableRawMode();
            return 1;
        }

        char buffer[128];
        double num = 0;

        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            num = atof(buffer);
        }
        pclose(fp);

        int total_time = (int)num;
        int *remaining_time = malloc(sizeof(int));
        *remaining_time = total_time;

        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            disableRawMode();
            return 1;
        }
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            disableRawMode();
            return 1;
        }
        int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if ((IMG_Init(img_flags) & img_flags) != img_flags) {
            fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
            SDL_Quit();
            return 1;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }

        Mix_Music *music = Mix_LoadMUS(music_file);
        if (!music) {
            printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }

        Mix_PlayMusic(music, loop ? -1 : 1);

        printf("'q' to quit, 's' to stop music, 'p' to resume music\n");
        char* name = getName(loop ? music_file : music_file);
        if (name == NULL) {
            printf("Playing %s\n\n", music_file);
        } else {
            printf("Playing %s\n\n", name);
        }

        free(name);
        pthread_t thread;
        pthread_create(&thread, NULL, clearLine, remaining_time);

        char input;
        while (1) {
            input = getchar();
            if (input == 'q') {
                stop = 2;
                break;
            }
            if (input == 's') {
                stop = 1;
                Mix_PauseMusic();
            }
            if (input == 'p' && stop == 1) {
                stop = 0;
                Mix_ResumeMusic();
            }
            SDL_Delay(100);
        }

        stop = 2;
        pthread_join(thread, NULL);
        free(remaining_time);
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        disableRawMode();
    } else {
        printf("Usage: %s <file>\n", argv[0]);
        printf("If you want more information, try the --help option\n");
    }
    return 1;
}

