//
// Created by lijian on 2018/12/24.
//

#include "PaAudio.h"


PaAudio::PaAudio(PaPlayStatus *paPlayStatus) {
    this->paPlayStatus = paPlayStatus;
    paQueue = new PaQueue(paPlayStatus);
}

PaAudio::~PaAudio() {

}
