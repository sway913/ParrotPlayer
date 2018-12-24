//
// Created by lijian on 2018/12/24.
//

#ifndef PARROTPLAYER_PAFFMPEG_H
#define PARROTPLAYER_PAFFMPEG_H

#include <pthread.h>
#include "AndroidLog.h"

extern "C" {
#include <libavformat/avformat.h>
};

class PaFFmpeg {
public:
    AVFormatContext *avFormatContext = NULL;
    pthread_t *decodeThread;

public:
    PaFFmpeg();

    ~PaFFmpeg();

    void prepared();

    void decodeFFmpegThread();
};


#endif //PARROTPLAYER_PAFFMPEG_H
