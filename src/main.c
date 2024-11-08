#include <stdio.h>
#include "qase.c"
#include <argp.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
    struct arguments arguments;
    memset(&arguments, 0, sizeof(struct arguments));

    arguments.loop = false;
    arguments.buffer_help = false;
    arguments.new_buffer = false;
    arguments.version = false;
    arguments.playlist = false;
    arguments.volume = MIX_MAX_VOLUME;
    arguments.playlist = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    char *music_file = arguments.file;
    int volume = arguments.volume;
    bool loop = arguments.loop;
    //bool version = arguments.version;
    //bool playlist = arguments.playlist;
    //bool buffer_help = arguments.buffer_help;
    bool new_buffer = arguments.new_buffer;

    if(music_file){
        init();
        enableRawMode();
        qase(music_file, NULL, new_buffer, volume, loop);

    }
}
