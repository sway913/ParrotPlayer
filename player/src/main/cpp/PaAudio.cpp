//
// Created by lijian on 2018/12/24.
//

#include "PaAudio.h"


PaAudio::PaAudio(PaPlayStatus *paPlayStatus, int sample_rate) {
    this->paPlayStatus = paPlayStatus;
    this->sample_rate = sample_rate;
    paQueue = new PaQueue(paPlayStatus);
    buffer = (uint8_t *) av_malloc(sample_rate * 2 * 2);
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

/**
 * 注入灵魂
 */
void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    PaAudio *paAudio = (PaAudio *) context;
    if (paAudio != NULL) {
        int bufferSize = paAudio->resampleAudio();
        if (bufferSize > 0) {
            (*paAudio->pcmBufferQueue)->Enqueue(paAudio->pcmBufferQueue, (char *) paAudio->buffer,
                                                bufferSize);
        }
    }
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


int PaAudio::resampleAudio() {
    while (paPlayStatus != NULL && !paPlayStatus->isExist) {
        avPacket = av_packet_alloc();
        if (paQueue->getAvPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        // 解码
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        avFrame = av_frame_alloc();
        // get soul avframe
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            if (avFrame->channels && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swrContext;
            swrContext = swr_alloc_set_opts(NULL,
                                            AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                            avFrame->sample_rate,
                                            avFrame->channel_layout,
                                            (AVSampleFormat) avFrame->format, avFrame->sample_rate,
                                            NULL,
                                            NULL);
            if (!swrContext || swr_init(swrContext) < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swrContext);
            }

            /**
             * 最终目标 data,data_size
             */
            int nb = swr_convert(swrContext, &buffer, avFrame->nb_samples,
                                 (const uint8_t **) avFrame->data, avFrame->nb_samples);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            LOGE("data size is %d", data_size);
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swrContext);
            break;
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }
    return data_size;
}
