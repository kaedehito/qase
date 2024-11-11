#include <stdio.h>
#include "qase.c"
#include <argp.h>
#include <sys/types.h>
#include <stdlib.h>

void ver() {
  printf("qase 3.0\n");
  printf("Copyright (c) 2024 futo ogasawara <pik6cs@gmail.com>\n");
  printf("This software is licensed under the MIT License\n");
}

int main(int argc, char *argv[]){
    struct arguments arguments;
    memset(&arguments, 0, sizeof(struct arguments));

    arguments.loop = false;
    arguments.buffer_help = false;
    arguments.new_buffer = false;
    arguments.version = false;
    arguments.volume = 70;
//    arguments.playlist = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    char *music_file = arguments.file;
    int volume = arguments.volume;
    bool loop = arguments.loop;
    bool version = arguments.version;
//    bool playlist = arguments.playlist;
    bool buffer_help = arguments.buffer_help;
    bool new_buffer = arguments.new_buffer;

    if(version){
	ver();
	return 0;	
    }
    if(buffer_help){
        help_display();
        return 0;
    }



/*    if(playlist == true){
        size_t line_count;
        char **list = read_playlist(music_file, &line_count);
        if(list == NULL){
            fprintf(stderr, "Playlist is NULL!");
            disableRawMode();
            return (int)line_count;
        }

        printf("\x1b[s");


        for(size_t i = 0; i < line_count; i++){
            char tex[100];
            int line_int = (int)i;
            line_int++;

            snprintf(tex, sizeof(tex),"The %dth item in the playlist\x1b[0m", line_int);

            init();
            enableRawMode();
            stop = 0;

            int music_duration = get_audio_duration(list[i]);
            qase(list[i],new_buffer, volume, loop, tex, music_duration);

	    
	    free(list[i]);
	    list[i] = NULL;

            for(int deline = 1; deline < 3; deline++){
                printf("\x1b[%dA\x1b[2K",deline);
            }

        }
        printf("\x1b[u");
        return 0;
    }
*/
    if(music_file){
        init();
        enableRawMode();
        int music_duration = get_audio_duration(music_file);
        qase(music_file,new_buffer, volume, loop, NULL, music_duration);

    }
}
