//
// Created by lijian on 2018/12/27.
//

#ifndef PARROTPLAYER_PAPLAYSTATUS_H
#define PARROTPLAYER_PAPLAYSTATUS_H

#include <pthread.h>

class PaPlayStatus {
public:
    bool isExist;
    bool seek;

    pthread_cond_t seekCon;

public:
    PaPlayStatus();

};

#endif //PARROTPLAYER_PAPLAYSTATUS_H
