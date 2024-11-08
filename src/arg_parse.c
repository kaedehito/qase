#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <argp.h>


void display_version(){
    printf("qase 3.0\n");
    printf("Copyright (c) 2024 futo ogasawara <pik6cs@gmail.com>\n");
    printf("This software is licensed under the MIT License\n");
}

struct arguments {
    char *file;
    bool buffer_help;
    bool loop;
    bool new_buffer;
    bool playlist;
    int volume;
    bool version;
};

// オプションの定義
static struct argp_option options[] = {
    {"buffer-help", 'f', 0, 0, "Shows qase help in a separate buffer", 0},
    {"loop", 'l', 0, 0, "Loop the music", 0},
    {"new-buffer", 'n', 0, 0, "Create a new buffer and run the qase", 0},
    {"playlist", 'p', 0, 0, "Reads a qase playlist (qpl) file and plays the playlist", 0},
    {"volume", 'v', "VOLUME", 0, "Change master volume", 0},
    {"version", 'V', 0, 0, "Display qase version", 0},
    { 0 }
};

// オプションの解析
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'f':
            arguments->buffer_help = true;
            break;
        case 'l':
            arguments->loop = true;
            break;
        case 'n':
            arguments->new_buffer = true;
            break;
        case 'p':
            arguments->playlist = true;
            break;
        case 'v':
            if (arg) {
                arguments->volume = atoi(arg);
                if (arguments->volume == 0 && strcmp(arg, "0") != 0) {
                    fprintf(stderr, "Invalid input value for volume: %s\n", arg);
                    exit(1);
                }
            }
            break;
        case 'V':
            display_version();
            exit(0);
            break;
        case ARGP_KEY_ARG:
            arguments->file = arg;
            break;
        case ARGP_KEY_END:
            if (!arguments->file) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, "FILE", "Extremely simple music playback program" , NULL, NULL, NULL};
