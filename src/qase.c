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

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
// include qase tool files
#include "arg_parse.c"
#include "display_time.c"
#include "get_music_length.c"
#include "help.c"
#include "init.c"
#include "messages.c"
#include "musicname.c"
#include "playlist.c"

void qase(char *music_file, bool new_buffer, int volume,
          bool loop, char *playlist_message, int music_duration) {


    // Play the music
    int play = Play(music_file, false);

    if (play == 1) {
      disableRawMode();
      exit(1);
    }

    printf("Welcome to qase! Press 'h' for help.\n\n\n");
    if (new_buffer) {
      printf("\x1b[?1049h");
      printf("\x1b[H");
    }

    //  chenge volume
    Mix_VolumeMusic(volume);

    char *name = getName(loop ? music_file : music_file);
    if(name == NULL){
        name = music_file;
    }
    char mess[100] = "\x1b[31mERROR\x1b[0m";
    // set display message
    if(playlist_message != NULL){
        snprintf(mess, sizeof(mess), "\x1b[35mNow Playing %s, %s\x1b[0m", name, playlist_message);

    }else{
        snprintf(mess, sizeof(mess), "\x1b[35mNow Playing %s\x1b[0m", name);
    }
    display_message(mess);
    free(name);



    // get music length
    int *time = (int *)&music_duration;

    pthread_t thread;
    pthread_create(&thread, NULL, display_time, time);

    // main loop
    char input = ' ';
    int running = true;

    while (running) {
      input = getchar();

      switch (input) {

      case 'h':            // help
        no_display = true; // Temporarily hide playback time
        help_display();
        no_display = false;

        break;

      case 'q': // quit
        display_message("\x1b[33mQuit\x1b[0m");
        stop = 2;
        running = false;

        break;

      case 's': // stop
        if (stop == 1) {
          display_message("\x1b[31mAlready in pose!\x1b[0m");
          break;
        }
        display_message("\x1b[33mPause Music\x1b[0m");
        stop = 1;
        Mix_PauseMusic();
        continue;

        break;

      case 'p': // play
        if (stop == 1) {
          display_message("\x1b[33mResume Music\x1b[0m");
          stop = 0;
          Mix_ResumeMusic();
          continue;
        } else {
          display_message("\x1b[31minvalid operation\x1b[0m");
          continue;
        }
        break;
      case 'j': // chenge volume(down)
        if (volume <= 0) {
          display_message("\x1b[32mAlready at the lowest volume\x1b[0m");
          continue;
          break;
        }

        volume--;

        char msg[100];
        snprintf(msg, sizeof(msg), "Chenge volume: %d", volume);
        display_message(msg);

        Mix_VolumeMusic(volume);
        continue;
        break;

      case 'k': // chenge volume(up)
        if (volume == MIX_MAX_VOLUME) {
          display_message("\x1b[31mAlready at maximum volume!\x1b[0m");
          continue;
          break;
        }
        char message[100];
        volume++;
        snprintf(message, sizeof(message), "Chenge volume: %d", volume);
        display_message(message);
        Mix_VolumeMusic(volume);
        continue;
        break;

      default:
        char tmt[100];
        snprintf(tmt, sizeof(tmt), "\x1b[33mUnknown command:\x1b[0m %c", input);
        display_message(tmt);
        break;

      }
      SDL_Delay(100);
    }
    // quit
    stop = 2;
    pthread_join(thread, NULL);
    Mix_FreeMusic(music); // music is defined at init.c
    Mix_CloseAudio();
    SDL_Quit();
    disableRawMode();
    printf("\x1b[?1049l");

    return;

}
