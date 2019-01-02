#include <jni.h>
#include <string>
#include "PaFFmpeg.h"

JavaVM *javaVM;
PaFFmpeg *paFFmpeg = NULL;
PaPlayStatus *paPlayStatus = NULL;

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    JNIEnv *jniEnv;
    if (javaVM->GetEnv((void **) (&jniEnv), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_lijian_app_parrotplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1prepared(JNIEnv *env, jobject instance,
                                                    jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    if (paFFmpeg == NULL) {
        PaCallJava *paCallJava = new PaCallJava(javaVM, env, instance);
        paPlayStatus = new PaPlayStatus();
        paFFmpeg = new PaFFmpeg(paPlayStatus, paCallJava, dataSource);
        paFFmpeg->prepared();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1start(JNIEnv *env, jobject instance) {
    if (paFFmpeg != NULL) {
        paFFmpeg->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1pause(JNIEnv *env, jobject instance) {
    if (paFFmpeg != NULL) {
        paFFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1resume(JNIEnv *env, jobject instance) {
    if (paFFmpeg != NULL) {
        paFFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1seek(JNIEnv *env, jobject instance, jint seconds) {
    if (paFFmpeg != NULL) {
        paFFmpeg->seek(seconds);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_lijian_app_player_ParrotPlayer_n_1getDuration(JNIEnv *env, jobject instance) {
    if (paFFmpeg != NULL) {
        return paFFmpeg->duration;
    }
    return 0;
}