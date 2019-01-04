package com.lijian.app.player;

import android.text.TextUtils;

import com.lijian.app.player.bean.PaTimeInfo;
import com.lijian.app.player.listener.OnPreparedListener;
import com.lijian.app.player.listener.OnResumeListener;
import com.lijian.app.player.listener.OnTimeInfoListener;

/**
 * @Description:
 * @Author: lijian
 * @Time: 2018/12/24
 */
public class ParrotPlayer {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }


    private String mDataSource;
    private OnPreparedListener mOnPreparedListener;
    private OnTimeInfoListener mOnTimeInfoListener;
    private OnResumeListener mOnResumeListener;
    private PaTimeInfo mPaTimeInfo;
    private int mVolumePercent;

    public String getDataSource() {
        return mDataSource;
    }

    public void setDataSource(String dataSource) {
        this.mDataSource = dataSource;
    }

    public OnPreparedListener getOnPreparedListener() {
        return mOnPreparedListener;
    }

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.mOnPreparedListener = onPreparedListener;
    }

    public OnTimeInfoListener getOnTimeInfoListener() {
        return mOnTimeInfoListener;
    }

    public void setOnTimeInfoListener(OnTimeInfoListener onTimeInfoListener) {
        this.mOnTimeInfoListener = onTimeInfoListener;
    }

    public OnResumeListener getmOnResumeListener() {
        return mOnResumeListener;
    }

    public void setOnResumeListener(OnResumeListener onResumeListener) {
        this.mOnResumeListener = onResumeListener;
    }

    public void prepared() {
        if (!TextUtils.isEmpty(mDataSource)) {
            n_prepared(mDataSource);
        }
    }

    public void start() {
        n_start();
    }

    public void pause() {
        n_pause();
        if (mOnResumeListener != null) {
            mOnResumeListener.onPause(true);
        }
    }

    public void resume() {
        n_resume();
        if (mOnResumeListener != null) {
            mOnResumeListener.onPause(false);
        }
    }

    public void seek(int seconds) {
        n_seek(seconds);
    }

    public int getDuration() {
        return n_getDuration();
    }

    public void setVolume(int percent) {
        if (percent >= 0 && percent <= 100) {
            mVolumePercent = percent;
            n_setVolume(percent);
        }
    }

    public void setMute(int mute) {
        n_setMute(mute);
    }

    private native void n_prepared(String dataSource);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();

    private native void n_seek(int seconds);

    private native int n_getDuration();

    private native void n_setVolume(int volume);

    private native void n_setMute(int mute);


    private void onCallPrepared() {
        if (mOnPreparedListener != null) {
            mOnPreparedListener.onPrepared();
        }
    }

    private void onCallTimeInfo(int totalTime, int currTime) {
        if (mOnTimeInfoListener != null) {
            if (mPaTimeInfo == null) {
                mPaTimeInfo = new PaTimeInfo();
            }
            mPaTimeInfo.setTotalTime(totalTime);
            mPaTimeInfo.setCurrTime(currTime);
            mOnTimeInfoListener.onTimeInfo(mPaTimeInfo);
        }
    }

}
