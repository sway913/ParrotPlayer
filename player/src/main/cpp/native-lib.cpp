#include <jni.h>
#include <string>
#include "PaFFmpeg.h"

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

    PaFFmpeg *paFFmpeg = new PaFFmpeg();
    paFFmpeg->prepared();
}