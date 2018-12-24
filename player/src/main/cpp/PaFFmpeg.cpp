//
// Created by lijian on 2018/12/24.
//

#include "PaFFmpeg.h"

void *decodeFFmpeg(void *data) {
    PaFFmpeg *paFFmpeg = (PaFFmpeg *) data;
    paFFmpeg->decodeFFmpegThread();
    pthread_exit(paFFmpeg->decodeThread);
}

void PaFFmpeg::prepared() {
    pthread_t *pthread;
    decodeThread = pthread;
    pthread_create(decodeThread, NULL, decodeFFmpeg, this);
}

void PaFFmpeg::decodeFFmpegThread() {
    LOGE("===> decodeFFmpegThread")
}

PaFFmpeg::PaFFmpeg() {
    LOGE("===> PaFFmpeg")
}

PaFFmpeg::~PaFFmpeg() {
    LOGE("===> ~PaFFmpeg")
}

