//
// Created by lijian on 2018/12/25.
//

#ifndef PARROTPLAYER_PAAUDIO_H
#define PARROTPLAYER_PAAUDIO_H

#include "PaPlayStatus.h"
#include "PaQueue.h"
#include "AndroidLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class PaAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;
    PaPlayStatus *paPlayStatus = NULL;
    PaQueue *paQueue;
    pthread_t threadPlay;

    int sample_rate = 0;

    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;
    // 混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    // pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    // 缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;


public:
    PaAudio(PaPlayStatus *paPlayStatus, int sample_rate);

    ~PaAudio();

    void play();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);
};


#endif //PARROTPLAYER_PAAUDIO_H
