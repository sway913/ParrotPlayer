package com.lijian.app.player;

import android.text.TextUtils;

import com.lijian.app.player.listener.OnPreparedListener;

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

    public void prepared() {
        if (!TextUtils.isEmpty(mDataSource)) {
            n_prepared(mDataSource);
        }
    }

    private native void n_prepared(String dataSource);

    private void onPrepared() {
        if (mOnPreparedListener != null) {
            mOnPreparedListener.onPrepared();
        }
    }

}
