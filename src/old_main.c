#include "help.c"
#include "image.c"
#include "init.c"
#include "messages.c"
#include "musicname.c"
#include "playlist.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <argp.h>
#include <id3tag.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

volatile int stop = 0;
volatile bool loop = false;
bool no_display = false;
int disable_cover = 0;
char *music_file = NULL;
bool newbuffer = false;
bool playlist = false;
int volume = SDL_MIX_MAXVOLUME;
bool playlistaddmode = false;

void ver() {
  printf("qase 3.0\n");
  printf("Copyright (c) 2024 futo ogasawara <pik6cs@gmail.com>\n");
  printf("This software is licensed under the MIT License\n");
}

struct arguments {
  char *args[1];
  int loop;
  int disable_cover;
  int volume;
  bool newbuffer;
  bool playlist;
};

static struct argp_option options[] = {
    {"loop", 'l', 0, 0, "Loop the music", 0},
    {"no-cover", 'c', 0, 0, "Disable cover image", 0},
    {"version", 'V', 0, 0, "Display qase version", 0},
    {"volume", 'v', "VOLUME", 0, "Chenge master volume", 0},
    {"new-buffer", 'n', 0, 0, "Create a new buffer and run the qase", 0},
    {"buffer-help", 'f', 0, 0, "Shows qase help in a separate buffer", 0},
    {"playlist", 'p', 0, 0,
     "Reads a qase playlist (qpl) file and plays the playlist", 0},
    {0, 0, 0, 0, NULL, 0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'l':
    arguments->loop = true;
    break;
  case 'c':
    arguments->disable_cover = 1;
    break;
  case 'V':
    ver();
    exit(0);
    break;
  case 'v':
    arguments->volume = atoi(arg);
    if ((*arg - 0) != 0 && arguments->volume == 0) {
      fprintf(stderr, "Invalid input value: %s\n", arg);
      exit(1);
    }
    break;
  case 'n':
    arguments->newbuffer = true;
    break;

  case 'f':
    enableRawMode();
    help_display();
    disableRawMode();
    exit(0);
    break;

  case 'p':
    arguments->playlist = true;
    break;

  case ARGP_KEY_ARG:
    if (state->arg_num >= 1) {
      argp_usage(state);
    }
    arguments->args[state->arg_num] = arg;
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

static struct argp argp = {
    options, parse_opt, "FILE", "Extremely simple music playback program",
    0,       NULL,      0};

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
    if (no_display == false) {
      if (minutes == 0) {
        printf("\033[F\033[K▶ %d seconds\n", seconds);
      } else {
        printf("\033[F\033[K▶ %d minute, %d seconds\n", minutes, seconds);
      }
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

void *qase(int *remaining_time, char *music__file, char playlist[]) {
  int play = Play(music__file, loop);
  if (play == 1) {
    disableRawMode();
    exit(1);
  }
  if (newbuffer) {
    printf("\x1b[?1049h");
    printf("\x1b[H");
  }

  Mix_VolumeMusic(volume);

  printf("'q' to quit, 's' to stop music, 'p' to resume music\n");
  char *name = getName(loop ? music__file : music__file);
  if (name == NULL) {
    printf("Playing %s\n\n\n", music__file);
  } else {
    printf("Playing %s\n\n\n", name);
  }
  if (playlist == NULL) {
    display_message("\x1b[36mPress 'h' for help\x1b[0m");
  } else {
    display_message(playlist);
  }

  free(name);
  pthread_t thread;
  pthread_create(&thread, NULL, clearLine, remaining_time);

  char input;
  while (1) {
    input = getchar();
    if (input == 'q') {
      display_message("\x1b[33mQuit\x1b[0m");
      stop = 2;
      break;
    }
    if (input == 's') {
      if (stop == 1) {
        display_message("\x1b[31mAlready in pose!\x1b[0m");
        continue;
      }
      display_message("\x1b[33mPause Music\x1b[0m");
      stop = 1;
      Mix_PauseMusic();
      continue;
    }
    if (input == 'p') {
      if (stop == 1) {
        display_message("\x1b[33mResume Music\x1b[0m");
        stop = 0;
        Mix_ResumeMusic();
        continue;
      }
      display_message("\x1b[31minvalid operation\x1b[0m");
      continue;
    }
    if (input == 'j') {
      if (volume <= 0) {
        display_message("\x1b[32mAlready at the lowest volume\x1b[0m");
        continue;
      }
      char text[100];
      volume--;
      snprintf(text, sizeof(text), "Chenge volume: %d", volume);
      display_message(text);
      Mix_VolumeMusic(volume);
      continue;
    }
    if (input == 'k') {
      if (volume == MIX_MAX_VOLUME) {
        display_message("\x1b[31mAlready at maximum volume!\x1b[0m");
        continue;
      }
      char text[100];
      volume++;
      snprintf(text, sizeof(text), "Chenge volume: %d", volume);
      display_message(text);
      Mix_VolumeMusic(volume);
      continue;
    } else if (input == 'h') {
      no_display = true;
      help_display();
      no_display = false;
    } else {
      char tmp_text[100];
      snprintf(tmp_text, sizeof(tmp_text), "\x1b[33mUnknown command:\x1b[0m %c",
               input);
      display_message(tmp_text);
    }

    SDL_Delay(100);
  }
  stop = 2;
  pthread_join(thread, NULL);
  free(remaining_time);
  Mix_FreeMusic(gmusic);
  Mix_CloseAudio();
  SDL_Quit();
  disableRawMode();
  printf("\x1b[?1049l");

  return 0;
}

int *get_remaining_time(char *file) {
  char text[200];
  snprintf(text, sizeof(text),
           "ffprobe -i \"%s\" -show_entries format=duration -v quiet -of "
           "csv=\"p=0\"",
           file);
  FILE *fp = popen(text, "r");

  if (fp == NULL) {
    printf("Failed to run command\n");
    disableRawMode();
    return NULL;
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

  return remaining_time;
}

int main(int argc, char *argv[]) {
  struct arguments arguments;
  arguments.loop = false;
  arguments.disable_cover = 0;
  arguments.volume = MIX_MAX_VOLUME;
  arguments.newbuffer = false;
  arguments.playlist = false;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  music_file = arguments.args[0];
  loop = arguments.loop;
  disable_cover = arguments.disable_cover;
  volume = arguments.volume;
  newbuffer = arguments.newbuffer;
  playlist = arguments.playlist;

  if (music_file) {

    init();

    if (playlist) {
      size_t line_count;
      char **list = read_playlist(music_file, &line_count);
      if (list == NULL) {
        disableRawMode();
        exit(1);
      }
      printf("\033[s");
      for (size_t i = 0; i < line_count; i++) {
        char tex[100];
        int temp = (int)i;
        temp++;
        snprintf(tex, sizeof(tex),
                 "\x1b[33mThe %dth item in the playlist\x1b[0m", temp);
        enableRawMode();
        stop = 0;
        int *remaining_time = get_remaining_time(list[i]);
        qase(remaining_time, list[i], tex);
        init();
        printf("\x1b[4A\x1b[2K");
      }
      printf("\033[u");
      SDL_Quit();
      disableRawMode();
      freeLines(list, line_count);
      return 0;
    }
    enableRawMode();
    int *remaining_time = get_remaining_time(music_file);
    qase(remaining_time, music_file, NULL);
  } else {
    printf("Usage: %s <file>\n", argv[0]);
    printf("If you want more information, try the --help option\n");
  }
  return 1;
}

