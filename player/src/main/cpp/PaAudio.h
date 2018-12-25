//
// Created by lijian on 2018/12/25.
//

#ifndef PARROTPLAYER_PAAUDIO_H
#define PARROTPLAYER_PAAUDIO_H

extern "C" {
#include <libavcodec/avcodec.h>
};

class PaAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;

public:
    PaAudio();

    ~PaAudio();
};


#endif //PARROTPLAYER_PAAUDIO_H
