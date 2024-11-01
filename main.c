#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>

volatile int stop = 0;

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
    int *int_num = (int *)arg;
    while (!stop && *int_num > 0) {
        sleep(1);
        if (*int_num > 0) {
            (*int_num)--;
            int minutes = *int_num / 60;
            int seconds = *int_num % 60;
            printf("\033[F\033[K"); 
            if (minutes == 0) {
                printf("%d seconds\n", seconds);
            } else {
                printf("%d minute, %d seconds\n", minutes, seconds);
            }
            fflush(stdout);
        }
    }
    free(arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        enableRawMode();

        // get music length
        char text[200];
        snprintf(text, sizeof(text), "ffprobe -i \"%s\" -show_entries format=duration -v quiet -of csv=\"p=0\"", argv[1]);
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
        int remaining_time = total_time; 

        // initialize SDL2
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            disableRawMode();
            return 1;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }
        Mix_Music *music = Mix_LoadMUS(argv[1]);
        if (!music) {
            printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }

        // Play The Music
        Mix_PlayMusic(music, 1);
        pthread_t thread;
        int *numm = malloc(sizeof(int));
        *numm = remaining_time;
        pthread_create(&thread, NULL, clearLine, numm);

        printf("'q' to quit, 's' to stop music, 'p' to start music\n");
        printf("Playing %s\n", argv[1]);

	printf("\n");
        char input;
        while (1) {
            input = getchar();

            if (input == 'q') {
                stop = 1;
                pthread_join(thread, NULL);
                break;
            }
            if (input == 's') {
                stop = 1;
                Mix_PauseMusic(); 
                remaining_time = *numm;
            }
            if (input == 'p') {
                if (stop) {
                    stop = 0; 
                    numm = malloc(sizeof(int));
                    *numm = remaining_time; 
                    pthread_create(&thread, NULL, clearLine, numm);
                    Mix_ResumeMusic(); 
                }
            }
            SDL_Delay(1000);
        }

        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        disableRawMode();
        return 0;
    } else {
        printf("Usage: %s <file>\n", argv[0]);
    }
    return 1;
}

