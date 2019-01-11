//
// Created by lijian on 2018/12/24.
//

#ifndef PARROTPLAYER_PAFFMPEG_H
#define PARROTPLAYER_PAFFMPEG_H

#include <pthread.h>
#include "AndroidLog.h"
#include "PaAudio.h"
#include "PaCallJava.h"
#include "PaPlayStatus.h"

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
    PaPlayStatus *paPlayStatus = NULL;
    int duration = 0;

public:
    PaFFmpeg(PaPlayStatus *paPlayStatus, PaCallJava *paCallJava, const char *url);

    ~PaFFmpeg();

    void prepared();

    void start();

    void decodeFFmpegThread();

    void pause();

    void resume();

    void seek(int seconds);

    void setVolume(int volumePercent);

    void setMute(int mute);

    void setPitch(float pitch);

    void setTempo(float tempo);

};


#endif //PARROTPLAYER_PAFFMPEG_H
