//
// Created by lijian on 2018/12/24.
//

#include "PaAudio.h"


PaAudio::PaAudio(PaPlayStatus *paPlayStatus, int sample_rate) {
    this->paPlayStatus = paPlayStatus;
    this->sample_rate = sample_rate;
    paQueue = new PaQueue(paPlayStatus);
}

PaAudio::~PaAudio() {

}

void *decodePlay(void *data) {
    PaAudio *paAudio = (PaAudio *) data;
    paAudio->initOpenSLES();
    pthread_exit(&paAudio->threadPlay);
}

void PaAudio::play() {
    pthread_create(&threadPlay, NULL, decodePlay, this);
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {

}

void PaAudio::initOpenSLES() {
    // create engine
    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    LOGE("create engine result: %s", result);

    // create 混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (result == SL_RESULT_SUCCESS) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSink = {&outputMix, 0};

    // 配置pcm格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queque = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,// 播放pcm格式的数据
            2,// 2个声道
            getCurrentSampleRateForOpensles(sample_rate),// 44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// 位数
            SL_PCMSAMPLEFORMAT_FIXED_16,// 位数
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// 立体声 （前左前右）
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {
            &android_queque,
            &pcm
    };

    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean reqs[1] = {SL_BOOLEAN_TRUE};

    // player
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSink, 1,
                                       ids, reqs);
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

    // 注册回调缓冲区，获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    pcmBufferCallBack(pcmBufferQueue, this);
}

int PaAudio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}
