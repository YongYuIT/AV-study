package com.hsdi.x264test;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.view.View;
import android.widget.Button;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class Main2Activity extends Activity {

    private Button btn_package;
    private Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        btn_package = (Button) findViewById(R.id.btn_package);
    }

    public void onClick(View v) {
        if (v.getId() == btn_package.getId()) {
            mPKThread.start();
        }
        if (v.getId() == R.id.btn_datas) {
            mFileThread.start();
        }
    }

    private void ChangeState() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                btn_package.setTextColor(0xff000000);
                btn_package.setClickable(true);
            }
        });
    }

    private Thread mPKThread = new Thread() {
        @Override
        public void run() {
            //FFmpegTest.ffmpeg_test_AV_package(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264", Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.mp4");
            FFmpegTest.ffmpeg_test_V_package(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264", Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.mp4");
        }
    };
    private Thread mFileThread = new Thread() {
        @Override
        public void run() {
            //删除旧数据
            File old_file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.mp4");
            if (old_file.exists()) {
                File tmp = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.tmp");
                old_file.renameTo(tmp);
                tmp.delete();
            }
            //拷贝数据
            File obj_file = new File("/sdcard/test_a.mp3");
            if (obj_file.exists()) {
                ChangeState();
                return;
            }
            if (!obj_file.getParentFile().exists()) {
                obj_file.getParentFile().mkdirs();
            }
            try {
                obj_file.createNewFile();
                InputStream inputStream = Main2Activity.this.getAssets()
                        .open("test_a.mp3");
                FileOutputStream fout = new FileOutputStream(obj_file);
                byte[] buffer = new byte[1024 * 10];
                while (true) {
                    int len = inputStream.read(buffer);
                    if (len == -1) {
                        break;
                    }
                    fout.write(buffer, 0, len);
                }
                inputStream.close();
                fout.close();
                ChangeState();
                return;
            } catch (Exception e) {
                return;
            }
        }
    };
}
