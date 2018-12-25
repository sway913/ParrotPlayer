//
// Created by lijian on 2018/12/24.
//

#ifndef PARROTPLAYER_PAFFMPEG_H
#define PARROTPLAYER_PAFFMPEG_H

#include <pthread.h>
#include "AndroidLog.h"
#include "PaAudio.h"
#include "PaCallJava.h"

extern "C" {
#include <libavformat/avformat.h>
};

class PaFFmpeg {
public:
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext = NULL;
    PaAudio *paAudio = NULL;
    PaCallJava *paCallJava = NULL;

public:
    PaFFmpeg(PaCallJava *paCallJava, const char *url);

    ~PaFFmpeg();

    void prepared();

    void decodeFFmpegThread();
};


#endif //PARROTPLAYER_PAFFMPEG_H
