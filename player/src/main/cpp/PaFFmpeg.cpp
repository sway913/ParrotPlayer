//
// Created by lijian on 2018/12/24.
//
#include "PaFFmpeg.h"

void *decodeFFmpeg(void *data) {
    PaFFmpeg *paFFmpeg = (PaFFmpeg *) data;
    paFFmpeg->decodeFFmpegThread();
    pthread_exit(&paFFmpeg->decodeThread);
}

void PaFFmpeg::prepared() {
    LOGE("===> prepared")
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

void PaFFmpeg::decodeFFmpegThread() {
    LOGD("===> decodeFFmpegThread")
    av_register_all();
    avformat_network_init();
    /**
     * CORE: av format context
     */
    avFormatContext = avformat_alloc_context();
    // check url
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("open url:%s failed", url);
        return;
    }
    // check stream
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("cant find stream info");
        return;
    }
    // find audio stream
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (paAudio == NULL) {
                paAudio = new PaAudio(paPlayStatus,
                                      avFormatContext->streams[i]->codecpar->sample_rate,
                                      paCallJava);
                paAudio->streamIndex = i;
                paAudio->codecPar = avFormatContext->streams[i]->codecpar;
                paAudio->duration = avFormatContext->duration / AV_TIME_BASE;
                paAudio->time_base = avFormatContext->streams[i]->time_base;
                duration = paAudio->duration;
            }
        }
    }

    // find codec
    AVCodec *avCodec = avcodec_find_decoder(paAudio->codecPar->codec_id);
    if (!avCodec) {
        LOGE("cant find avcodec");
        return;
    }
    paAudio->avCodecContext = avcodec_alloc_context3(avCodec);
    if (!paAudio->avCodecContext) {
        LOGE("cant alloc avcodec context");
        return;
    }
    if (avcodec_parameters_to_context(paAudio->avCodecContext, paAudio->codecPar) < 0) {
        LOGE("cant fill params to codec context");
        return;
    }
    if (avcodec_open2(paAudio->avCodecContext, avCodec, 0) < 0) {
        LOGE("cant open audio streams");
        return;
    }

    paCallJava->callOnPrepared(CHILD_THREAD);
}

PaFFmpeg::PaFFmpeg(PaPlayStatus *paPlayStatus, PaCallJava *paCallJava, const char *url) {
    this->paPlayStatus = paPlayStatus;
    this->paCallJava = paCallJava;
    this->url = url;
}

PaFFmpeg::~PaFFmpeg() {
    LOGE("===> ~PaFFmpeg")
}

void PaFFmpeg::start() {
    // audio play
    if (paAudio == NULL) {
        LOGE("paAudio==NULL");
        return;
    }
    paAudio->play();

    // decode
    LOGE("decode start")
    int count = 0;
    while (paPlayStatus != NULL && !paPlayStatus->isExist) {
        if (paPlayStatus->seek) {
            LOGE("PaFFmpeg decode seek wait")
            pthread_cond_wait(&paPlayStatus->seekCon, NULL);
            LOGE("PaFFmpeg decode seek up")
        }
        if (paAudio->paQueue->getQueueSize() > 40) {
            continue;
        }
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(avFormatContext, avPacket) == 0) {
            if (avPacket->stream_index == paAudio->streamIndex) {
                LOGE("decode %d 帧", count);
                count++;
                // 入队
                paAudio->paQueue->putAvPacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            LOGE("av_read_frame(avFormatContext, avPacket) != 0");
            break;
        }
    }

    LOGE("decode end")

}

void PaFFmpeg::pause() {
    if (paAudio != NULL) {
        paAudio->pause();
    }
}

void PaFFmpeg::resume() {
    if (paAudio != NULL) {
        paAudio->resume();
    }
}

void PaFFmpeg::seek(int seconds) {
    LOGE("seek time :%d", seconds)
    if (duration <= 0) {
        return;
    }
    if (seconds >= 0 && seconds <= duration) {
        if (paAudio != NULL) {
            paAudio->pause();
            paPlayStatus->seek = true;
            // reset time
            paAudio->now_time = 0;
            paAudio->last_time = 0;
            //
            paAudio->paQueue->clearAvPacket();

            int64_t seekTime = seconds * AV_TIME_BASE;
            avformat_seek_file(avFormatContext, -1, INT64_MIN, seekTime, INT64_MAX, 0);
            paPlayStatus->seek = false;
            pthread_cond_broadcast(&paPlayStatus->seekCon);
            paAudio->resume();
            LOGE("seek signal")
        }
    }
}

void PaFFmpeg::setVolume(int volumePercent) {
    if (paAudio != NULL) {
        paAudio->setVolume(volumePercent);
    }
}

void PaFFmpeg::setMute(int mute) {
    if (paAudio != NULL) {
        paAudio->setMute(mute);
    }
}

void PaFFmpeg::setPitch(float pitch) {
    if (paAudio != NULL) {
        paAudio->setPitch(pitch);
    }
}

void PaFFmpeg::setTempo(float tempo) {
    if (paAudio != NULL) {
        paAudio->setTempo(tempo);
    }
}



