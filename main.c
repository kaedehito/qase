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
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>

volatile int stop = 0;
volatile int loop = 0;

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
    if (loop == 1){
	    printf("\033[F\033[K");
	    printf("loop playback\n");
    }
    int *int_num = (int *)arg;
    
    while (!stop && *int_num > 0) {
        sleep(1);
	if (loop == 1){
		printf("\033[F\033[K"); 
		printf("loop playback");
		continue;
	}
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
        }else{
		break;
	}
    }
    free(arg);
    return NULL;
}


void help(char* arg){
	printf("qase 1.0\n");
	printf("Usage: %s [option] <file>\n", arg);
	printf("\nOption: \n");
	printf("\t-l: Loop the music\n");
	printf("\n");
	printf("Copyright (c) 2024 futo ogasawara <pik6cs@gmail.com>\n");
	printf("This software is licensed under the MIT License\n");
	
}


int main(int argc, char *argv[]) {
    if (argc >= 2) {
	if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
		help(argv[0]);
		return 0;
	}

	
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
	Mix_Music *music = NULL;
	if(strcmp(argv[1], "-l") == 0){
		music = Mix_LoadMUS(argv[2]);
		loop = 1;
	}else{
		music = Mix_LoadMUS(argv[1]);
	}
        if (!music) {
            printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
            disableRawMode();
            return 1;
        }

        // Play The Music
	if(strcmp(argv[1], "-l")){
		Mix_PlayMusic(music, -1);
	}
        Mix_PlayMusic(music, 1);
        pthread_t thread;
        int *numm = malloc(sizeof(int));
        *numm = remaining_time;
        pthread_create(&thread, NULL, clearLine, numm);

        printf("'q' to quit, 's' to stop music, 'p' to start music\n");
	if (loop == 1){
        	printf("Playing %s\n", argv[2]);
	}else{
        	printf("Playing %s\n", argv[1]);
	}

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
	printf("If you want more information, try the --help option\n");
    }
    return 1;
}

