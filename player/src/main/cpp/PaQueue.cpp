//
// Created by lijian on 2018/12/27.
//
#include "PaQueue.h"
#include "AndroidLog.h"

/**
 * note:push完，主动通知消费者
 */
int PaQueue::putAvPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutexAvPacket);
    queueAvPacket.push(avPacket);
//    LOGE("push an packet into queue,queue size: %d", queueAvPacket.size())
    pthread_mutex_unlock(&mutexAvPacket);
    // notify
    pthread_cond_signal(&condAvPacket);
    return 0;
}

int PaQueue::getAvPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutexAvPacket);

    while (paPlayStatus != NULL && !paPlayStatus->isExist) {
        if (queueAvPacket.size() > 0) {
            AVPacket *avPacketFront = queueAvPacket.front();
            if (av_packet_ref(avPacket, avPacketFront) == 0) {
                queueAvPacket.pop();
            }
            av_packet_free(&avPacketFront);
            av_free(avPacketFront);
            avPacketFront = NULL;
//            LOGE("pop an packet from queue,queue size: %d", queueAvPacket.size())
            break;
        } else {
            pthread_cond_wait(&condAvPacket, &mutexAvPacket);
        }
    }

    pthread_mutex_unlock(&mutexAvPacket);
    return 0;
}

int PaQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexAvPacket);
    size = queueAvPacket.size();
    pthread_mutex_unlock(&mutexAvPacket);
    return size;
}

PaQueue::PaQueue(PaPlayStatus *paPlayStatus) {
    this->paPlayStatus = paPlayStatus;
    pthread_mutex_init(&mutexAvPacket, NULL);
    pthread_cond_init(&condAvPacket, NULL);
}

PaQueue::~PaQueue() {

}

void PaQueue::clearAvPacket() {
    pthread_mutex_lock(&mutexAvPacket);
    while (!queueAvPacket.empty()) {
        AVPacket *avPacket = queueAvPacket.front();
        queueAvPacket.pop();
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    pthread_mutex_unlock(&mutexAvPacket);
}
