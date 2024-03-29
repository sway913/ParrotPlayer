//
// Created by lijian on 2018/12/25.
//

#ifndef PARROTPLAYER_PAAUDIO_H
#define PARROTPLAYER_PAAUDIO_H

#include "PaPlayStatus.h"
#include "PaQueue.h"
#include "AndroidLog.h"
#include "PaCallJava.h"
#include "SoundTouch.h"

using namespace soundtouch;

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

    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int sample_rate = 0;
    uint8_t *buffer = NULL;
    int ret = 0;
    int data_size = 0;

    int duration = 0;
    double now_time; //当前frame时间
    double last_time;
    AVRational time_base;

    PaCallJava *paCallJava = NULL;

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
    // volume
    SLVolumeItf pcmVolume = NULL;
    SLMuteSoloItf pcmMute = NULL;
    int volumePercent = 100;
    // 缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;
    // soundtouch
    SoundTouch *soundTouch = NULL;
    float pitch = 1.0f;
    float tempo = 1.0f;
    uint8_t *out_buffer = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    int nb;

public:
    PaAudio(PaPlayStatus *paPlayStatus, int sample_rate, PaCallJava *paCallJava);

    ~PaAudio();

    void play();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);

    int resampleAudio(void **pcmBuffer);

    void pause();

    void resume();

    void setVolume(int percent);

    void setMute(int mute);

    void setTempo(float tempo);

    void setPitch(float pitch);

    int getSoundTouchData();

    int getPCMDB(char *pcmdata, size_t pcmsize);
};


#endif //PARROTPLAYER_PAAUDIO_H
