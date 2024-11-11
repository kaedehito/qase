#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <stdio.h>
#include <stdlib.h>


int get_audio_duration(const char *filename){
    av_log_set_level(AV_LOG_ERROR);
    AVFormatContext *fmt_ctx = NULL;
    double duration;

    if(avformat_open_input(&fmt_ctx, filename, NULL, NULL) != 0){
        fprintf(stderr, "Failed to open file\n");
        exit(1);
        return -1;
    }

    if(avformat_find_stream_info(fmt_ctx, NULL) < 0){
        fprintf(stderr, "Failed to get stream info");
        exit(1);
        return -1;
    }

    duration = fmt_ctx->duration / (double)AV_TIME_BASE;

    avformat_close_input(&fmt_ctx);

    return (int)duration;
}
