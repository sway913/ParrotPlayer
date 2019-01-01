//
// Created by lijian on 2018/12/24.
//

#include "PaCallJava.h"
#include "AndroidLog.h"


PaCallJava::PaCallJava(JavaVM *javaVM, JNIEnv *jniEnv, jobject obj) {

    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jb = jniEnv->NewGlobalRef(obj);

    jclass jlz = jniEnv->GetObjectClass(jb);
    if (!jlz) {
        LOGE("cant get jclass from");
        return;
    }
    jmethodID_onCallPrepared = jniEnv->GetMethodID(jlz, "onCallPrepared", "()V");
    jmethodID_onCallTimeInfo = jniEnv->GetMethodID(jlz, "onCallTimeInfo", "(II)V");
}

void PaCallJava::callOnPrepared(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jb, jmethodID_onCallPrepared);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("onCallPrepared attach jni fail");
            return;
        }
        jniEnv->CallVoidMethod(jb, jmethodID_onCallPrepared);
        javaVM->DetachCurrentThread();
    }
}

void PaCallJava::callOnTimeInfo(int type, int totalTime, int currTime) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jb, jmethodID_onCallTimeInfo,totalTime,currTime);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("onCallTimeInfo attach jni fail");
            return;
        }
        jniEnv->CallVoidMethod(jb, jmethodID_onCallTimeInfo,totalTime,currTime);
        javaVM->DetachCurrentThread();
    }
}


