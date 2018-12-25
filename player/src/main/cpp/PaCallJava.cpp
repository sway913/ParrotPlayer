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
    jmethodID_onprepared = jniEnv->GetMethodID(jlz, "onPrepared", "()V");
}

void PaCallJava::callOnPrepared(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jb, jmethodID_onprepared);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("callOnPrepared attach jni fail");
            return;
        }
        jniEnv->CallVoidMethod(jb, jmethodID_onprepared);
        javaVM->DetachCurrentThread();
    }
}


