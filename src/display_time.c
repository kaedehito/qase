#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// stop flag
// 0 = play, 1 = stop, 2 = quit
volatile int stop = 0;
bool no_display = false;
bool loop = false;

void *display_time(void *arg) {
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
