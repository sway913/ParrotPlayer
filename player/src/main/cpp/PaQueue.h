//
// Created by lijian on 2018/12/27.
//

#ifndef PARROTPLAYER_PAQUEUE_H
#define PARROTPLAYER_PAQUEUE_H

#include <pthread.h>
#include "queue"
#include "PaPlayStatus.h"

extern "C" {
#include <libavcodec/avcodec.h>
};


class PaQueue {
public:
    std::queue<AVPacket *> queueAvPacket;
    pthread_mutex_t mutexAvPacket;
    pthread_cond_t condAvPacket;
    PaPlayStatus *paPlayStatus = NULL;

public:
    PaQueue(PaPlayStatus *paPlayStatus);

    ~PaQueue();

    int putAvPacket(AVPacket *avPacket);

    int getAvPacket(AVPacket *avPacket);

    int getQueueSize();
};

#endif //PARROTPLAYER_PAQUEUE_H
