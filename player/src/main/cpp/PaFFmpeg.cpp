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
                paAudio = new PaAudio();
                paAudio->streamIndex = i;
                paAudio->codecPar = avFormatContext->streams[i]->codecpar;
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

PaFFmpeg::PaFFmpeg(PaCallJava *paCallJava, const char *url) {
    LOGE("===> PaFFmpeg")
    this->paCallJava = paCallJava;
    this->url = url;
}

PaFFmpeg::~PaFFmpeg() {
    LOGE("===> ~PaFFmpeg")
}



