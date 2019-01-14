//
// Created by lijian on 2018/12/25.
//

#ifndef PARROTPLAYER_PACALLJAVA_H
#define PARROTPLAYER_PACALLJAVA_H

#include <jni.h>

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class PaCallJava {


public:
    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jb;
    jmethodID jmethodID_onCallPrepared;
    jmethodID jmethodID_onCallTimeInfo;
    jmethodID jmethodID_onCallVolumeDB;

public:
    PaCallJava(JavaVM *javaVM, JNIEnv *jniEnv, jobject jb);

    ~PaCallJava();

    void callOnPrepared(int type);

    void callOnTimeInfo(int type, int totalTime, int currTime);

    void callOnVolumeDB(int type, int db);
};


#endif //PARROTPLAYER_PACALLJAVA_H
