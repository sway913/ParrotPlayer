//
// Created by lijian on 2018/12/25.
//

#ifndef PARROTPLAYER_PAAUDIO_H
#define PARROTPLAYER_PAAUDIO_H

#include "PaPlayStatus.h"
#include "PaQueue.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class PaAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;
    PaPlayStatus *paPlayStatus = NULL;
    PaQueue *paQueue;

public:
    PaAudio(PaPlayStatus *paPlayStatus);

    ~PaAudio();
};


#endif //PARROTPLAYER_PAAUDIO_H
