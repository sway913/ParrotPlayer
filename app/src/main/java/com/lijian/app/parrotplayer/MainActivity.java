package com.lijian.app.parrotplayer;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.lijian.app.player.ParrotPlayer;
import com.lijian.app.player.bean.PaTimeInfo;
import com.lijian.app.player.listener.OnPreparedListener;
import com.lijian.app.player.listener.OnResumeListener;
import com.lijian.app.player.listener.OnTimeInfoListener;
import com.lijian.app.player.utils.PaTimeUtil;

public class MainActivity extends AppCompatActivity {
    private static String TAG = "MainActivity";

    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};

    ParrotPlayer parrotPlayer;
    TextView tvTime;
    SeekBar sbSeek;
    TextView tvVolume;
    SeekBar sbVolume;
    boolean fromUser;
    int position;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvTime = findViewById(R.id.tv_time);
        sbSeek = findViewById(R.id.sb_seek);
        tvVolume = findViewById(R.id.tv_volume);
        sbVolume = findViewById(R.id.sb_volume);

        verifyStoragePermissions(this);

        parrotPlayer = new ParrotPlayer();
        parrotPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.e(TAG, "---> onPrepared,start play music");
                parrotPlayer.start();
            }
        });
        parrotPlayer.setOnTimeInfoListener(new OnTimeInfoListener() {
            @Override
            public void onTimeInfo(final PaTimeInfo timeInfo) {
                tvTime.post(new Runnable() {
                    @Override
                    public void run() {
                        sbSeek.setProgress((int) (100 * (timeInfo.getCurrTime() * 1.0f / timeInfo.getTotalTime())));
                        tvTime.setText(PaTimeUtil.secdsToDateFormat(timeInfo.getTotalTime(), timeInfo.getTotalTime()) + "/" +
                                PaTimeUtil.secdsToDateFormat(timeInfo.getCurrTime(), timeInfo.getTotalTime()));
                    }
                });
            }
        });
        parrotPlayer.setOnResumeListener(new OnResumeListener() {
            @Override
            public void onPause(boolean pause) {
                Log.e(TAG, "---> play status is pause:" + pause);
            }
        });

        sbSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                fromUser = b;
                if (parrotPlayer.getDuration() > 0 && b) {
                    position = (int) (parrotPlayer.getDuration() * i * 1.0f / 100);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if (fromUser) {
                    parrotPlayer.seek(position);
                }
            }
        });

        tvVolume.setText("音量:100%");
        sbVolume.setProgress(100);
        sbVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                parrotPlayer.setVolume(i);
                tvVolume.setText("音量:" + i + "%");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }


    public static void verifyStoragePermissions(Activity activity) {

        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(activity,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, 6);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public void onBegin(View view) {
//        parrotPlayer.setDataSource("/sdcard/mid.mp3");
        parrotPlayer.setDataSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        parrotPlayer.prepared();
    }

    /**
     * 去暂停
     *
     * @param view
     */
    public void onPause(View view) {
        parrotPlayer.pause();
    }

    /**
     * 去播放
     *
     * @param view
     */
    public void onResume(View view) {
        parrotPlayer.resume();
    }


    public void onVolumeLeft(View view) {
        parrotPlayer.setMute(0);
    }

    public void onVolumeRight(View view) {
        parrotPlayer.setMute(1);
    }

    public void onVolumeStereo(View view) {
        parrotPlayer.setMute(2);
    }
}
