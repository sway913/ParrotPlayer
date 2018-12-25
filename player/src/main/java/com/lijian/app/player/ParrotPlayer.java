package com.lijian.app.player;

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


    public String getDataSource() {
        return mDataSource;
    }

    public void setDataSource(String dataSource) {
        this.mDataSource = dataSource;
    }

    public void prepared() {
        n_prepared("hello");
    }

    private native void n_prepared(String dataSource);

    private void onPrepared() {

    }

}
