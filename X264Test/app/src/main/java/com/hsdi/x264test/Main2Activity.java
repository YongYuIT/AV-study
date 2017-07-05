package com.hsdi.x264test;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;

public class Main2Activity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
    }

    public void onClick(View v) {
        if (v.getId() == R.id.btn_package) {
            mThread.start();
        }
    }

    private static Thread mThread = new Thread() {
        @Override
        public void run() {
            FFmpegTest.ffpmeg_pack_h264_2_mp4(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264", Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.mp4");
        }
    };
}
