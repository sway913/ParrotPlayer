package com.lijian.app.parrotplayer;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.lijian.app.player.ParrotPlayer;
import com.lijian.app.player.listener.OnPreparedListener;

public class MainActivity extends AppCompatActivity {
    private static String TAG = "MainActivity";

    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};

    ParrotPlayer parrotPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        parrotPlayer = new ParrotPlayer();
        verifyStoragePermissions(this);
    }

    public void start(View view) {
        parrotPlayer.setDataSource("/sdcard/mid.mp3");
        parrotPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.e(TAG, "onPrepared");
            }
        });
        parrotPlayer.prepared();
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
}
