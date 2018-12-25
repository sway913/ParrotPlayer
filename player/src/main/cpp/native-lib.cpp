#include <jni.h>
#include <string>
#include "PaFFmpeg.h"

JavaVM *javaVM;
PaFFmpeg *paFFmpeg = NULL;

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
    env->ReleaseStringUTFChars(dataSource_, dataSource);

    if (paFFmpeg == NULL) {
        PaCallJava *paCallJava = new PaCallJava(javaVM, env, instance);
        paFFmpeg = new PaFFmpeg(paCallJava, dataSource);
        paFFmpeg->prepared();
    }
}