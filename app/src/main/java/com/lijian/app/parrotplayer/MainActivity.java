package com.lijian.app.parrotplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.lijian.app.player.ParrotPlayer;

public class MainActivity extends AppCompatActivity {

    ParrotPlayer parrotPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        parrotPlayer = new ParrotPlayer();
    }

    public void start(View view) {
        parrotPlayer.prepared();
    }
}
