//
// Created by lijian on 2018/12/27.
//

#include "PaPlayStatus.h"

PaPlayStatus::PaPlayStatus() {
    isExist = false;
    pthread_cond_init(&seekCon, NULL);
}
