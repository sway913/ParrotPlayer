package com.lijian.app.player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;

import com.lijian.app.player.bean.PaTimeInfo;
import com.lijian.app.player.listener.OnPreparedListener;
import com.lijian.app.player.listener.OnRecordTimeListener;
import com.lijian.app.player.listener.OnResumeListener;
import com.lijian.app.player.listener.OnTimeInfoListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * @Description:
 * @Author: lijian
 * @Time: 2018/12/24
 */
public class ParrotPlayer {

    public static final String TAG = "ParrotPlayer";

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
    private boolean mInitMediaCodec;

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
            n_setVolume(percent);
        }
    }

    public void setMute(int mute) {
        n_setMute(mute);
    }

    public void setTempo(float tempo) {
        n_setTempo(tempo);
    }

    public void setPitch(float pitch) {
        n_setPitch(pitch);
    }

    public void startRecord(File outFile) {
        if (!mInitMediaCodec) {

        }
    }


    private native void n_prepared(String dataSource);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();

    private native void n_seek(int seconds);

    private native int n_getDuration();

    private native void n_setVolume(int volume);

    private native void n_setMute(int mute);

    private native void n_setTempo(float tempo);

    private native void n_setPitch(float pitch);


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


    private int aacSampleRate = 4;
    private int audioSampleRate;
    private int recordTime;
    private OnRecordTimeListener onRecordTimeListener;
    private MediaFormat mediaFormat;
    private MediaCodec encoder;
    private FileOutputStream fileOutputStream;
    private MediaCodec.BufferInfo bufferInfo;

    private void initMediaCodec(int sampleRate, File outFile) {
        aacSampleRate = getADTSsamplerate(sampleRate);
        try {
            mediaFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, 2);
            mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);

            encoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            if (encoder == null) {
                Log.e(TAG, "create encoder error");
            }
            encoder.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            recordTime = 0;
            fileOutputStream = new FileOutputStream(outFile);
            bufferInfo = new MediaCodec.BufferInfo();
            encoder.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void encodePcmToAAC(int size, byte[] buffer) {
        if (encoder != null && buffer != null) {
            recordTime += (int) (size * 1.0f / (audioSampleRate * 2 * 2));
            Log.e(TAG, "recordTime:" + recordTime);
            if (onRecordTimeListener != null) {
                onRecordTimeListener.onRecordTime(recordTime);
            }

            // input
            int inputBufferIndex = encoder.dequeueInputBuffer(0);
            if (inputBufferIndex >= 0) {
                ByteBuffer byteBuffer = encoder.getInputBuffer()[inputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(buffer);
                encoder.queueInputBuffer(inputBufferIndex, 0, size, 0, 0);
            }

            // output
            int index = encoder.dequeueOutputBuffer(bufferInfo, 0);
            while (index > 0) {
                try {
                    int perPcmSize = bufferInfo.size + 7;
                    byte[] outByteBuffer = new byte[perPcmSize];

                    ByteBuffer byteBuffer = encoder.getOutputBuffer()[index];
                    byteBuffer.position(bufferInfo.offset);
                    byteBuffer.limit(bufferInfo.offset + bufferInfo.size);
                    // addd adts header

                    byteBuffer.get(outByteBuffer, 7, bufferInfo.size);
                    byteBuffer.position(bufferInfo.offset); // reset
                    fileOutputStream.write(outByteBuffer, 0, perPcmSize);

                    encoder.releaseOutputBuffer(index,false);
                    index=encoder.dequeueOutputBuffer(bufferInfo,0);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }
    }


    private int getADTSsamplerate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }


}
