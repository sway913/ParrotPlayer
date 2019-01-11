//
// Created by lijian on 2018/12/24.
//

#include "PaAudio.h"


PaAudio::PaAudio(PaPlayStatus *paPlayStatus, int sample_rate, PaCallJava *paCallJava) {
    this->paPlayStatus = paPlayStatus;
    this->sample_rate = sample_rate;
    this->paCallJava = paCallJava;
    paQueue = new PaQueue(paPlayStatus);
    buffer = (uint8_t *) av_malloc(sample_rate * 2 * 2);

    sampleBuffer = static_cast<SAMPLETYPE *>(malloc(sample_rate * 2 * 2));

    soundTouch = new SoundTouch();
    soundTouch->setSampleRate(sample_rate);
    soundTouch->setChannels(2);
    soundTouch->setPitch(1.0f);
    soundTouch->setTempo(1.0f);
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
//        int bufferSize = paAudio->resampleAudio(reinterpret_cast<void **>(&paAudio->sampleBuffer));
        int bufferSize = paAudio->getSoundTouchData();
        if (bufferSize > 0) {
            // tell timeinfo
            if (paAudio->now_time - paAudio->last_time >= 0.1) {
                paAudio->last_time = paAudio->now_time;
                paAudio->paCallJava->callOnTimeInfo(CHILD_THREAD, paAudio->duration,
                                                    paAudio->now_time);
            }
            // record

            // volume db

            (*paAudio->pcmBufferQueue)->Enqueue(paAudio->pcmBufferQueue,
                                                (char *) paAudio->sampleBuffer, bufferSize * 2 * 2);
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

    const SLInterfaceID ids[4] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE,
                                  SL_IID_MUTESOLO};
    const SLboolean req[4] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSink, 4,
                                       ids, req);
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

    // volume
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmVolume);
    // mute
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmMute);

    // 注册回调缓冲区，获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);

    setVolume(volumePercent);
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


int PaAudio::resampleAudio(void **pcmBuffer) {
    while (paPlayStatus != NULL && !paPlayStatus->isExist) {
        if (paPlayStatus->seek) {
//            LOGE("PaAudio decode seek wait")
            pthread_cond_wait(&paPlayStatus->seekCon, NULL);
//            LOGE("PaAudio decode seek up")
        }

        avPacket = av_packet_alloc();
        if (paQueue->getAvPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        // 解码
        ret = avcodec_send_packet(avCodecContext, avPacket); // packet送到解码器
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        avFrame = av_frame_alloc();
        // get soul avframe
        ret = avcodec_receive_frame(avCodecContext, avFrame); // 取出解码后的frame
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
            nb = swr_convert(swrContext, &buffer, avFrame->nb_samples,
                             (const uint8_t **) avFrame->data, avFrame->nb_samples);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            // cal time
            now_time = avFrame->pts * av_q2d(time_base);

            *pcmBuffer = buffer;

//            LOGE("data size is %d", data_size);
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

void PaAudio::pause() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void PaAudio::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void PaAudio::setVolume(int percent) {
    if (pcmVolume != NULL) {
        if (percent > 30) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -40);
        } else {
            (*pcmVolume)->SetVolumeLevel(pcmVolume, (100 - percent) * -100);
        }
    }
}

void PaAudio::setMute(int mute) {
    if (pcmMute != NULL) {
        switch (mute) {
            case 0:
                // left
                (*pcmMute)->SetChannelMute(pcmMute, 0, true);
                (*pcmMute)->SetChannelMute(pcmMute, 1, false);
                break;
            case 1:
                // right
                (*pcmMute)->SetChannelMute(pcmMute, 0, false);
                (*pcmMute)->SetChannelMute(pcmMute, 1, true);
                break;
            case 2:
                // stereo
                (*pcmMute)->SetChannelMute(pcmMute, 0, false);
                (*pcmMute)->SetChannelMute(pcmMute, 1, false);
                break;
        }
    }
}

void PaAudio::setTempo(float tempo) {
    if (soundTouch != NULL) {
        this->tempo = tempo;
        soundTouch->setTempo(tempo);
    }
}

void PaAudio::setPitch(float pitch) {
    if (soundTouch != NULL) {
        this->pitch = pitch;
        soundTouch->setPitch(pitch);
    }
}

int PaAudio::getSoundTouchData() {
    while (paPlayStatus != NULL && !paPlayStatus->isExist) {
        int num = 0;
        data_size = resampleAudio(reinterpret_cast<void **>(&out_buffer));
        if (data_size > 0) {
            for (int i = 0; i < data_size / 2 + 1; i++) {
                sampleBuffer[i] = (out_buffer[i * 2] | ((out_buffer[i * 2 + 1]) << 8));
            }
            soundTouch->putSamples(sampleBuffer, nb);
            num = soundTouch->receiveSamples(sampleBuffer, data_size / 4);
        } else {
            soundTouch->flush();
        }
        if (num != 0) {
            return num;
        }
    }
    return 0;
}