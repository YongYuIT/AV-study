package com.thinking.pcmtest;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onClick(View view) {
        if (view.getId() == R.id.btn_record_start) {
            PCMTool.doRecord();
        } else if (view.getId() == R.id.btn_record_end) {
            PCMTool.stopRecord();
        } else if (view.getId() == R.id.btn_play) {
            PCMTool.play();
        }
    }
}

class PCMTool {
    //录音参数设置-----------------------start
    private final static int audioSource = MediaRecorder.AudioSource.MIC;//设置录音源为麦克风
    private final static int sampleRateInHz = 44100;//设置采样率，44100是比较常见的标准；某些设备可能还支持22050，16000，11025
    private final static int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_STEREO;//设置音道数，CHANNEL_CONFIGURATION_STEREO为立体音（双音道）
    private final static int audioFormat = AudioFormat.ENCODING_PCM_16BIT;//设置编码制式，表征编码精度。16bit表示量化时，每帧音频数据值用16位来表示。
    //这样产生的PCM数据的码流就是 sampleRateInHz*channelConfig*audioFormat 即 44100*2*16 bit/s
    private static byte[] pcmCache = null;
    //录音参数设置-----------------------end

    private static AudioRecord audioRecord;
    private static AudioTrack audioTrack;
    private static boolean isRecording = false;

    private static ExecutorService pool = Executors.newFixedThreadPool(10);

    public static void doRecord() {
        pool.submit(new Runnable() {
            @Override
            public void run() {
                getPCM();
            }
        });
    }

    public static void stopRecord() {
        isRecording = false;
    }

    public static void play() {
        pool.submit(new Runnable() {
            @Override
            public void run() {
                playPCM();
            }
        });
    }

    private static void getPCM() {
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "//test_pcm.pcm");
        if (file.exists()) {
            file.delete();
        }
        try {
            file.createNewFile();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        FileOutputStream fileOut = null;
        try {
            fileOut = new FileOutputStream(file);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        //获取符合系统要求的最小缓冲区
        pcmCache = new byte[AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat)];
        audioRecord = new AudioRecord(audioSource, sampleRateInHz, channelConfig, audioFormat, pcmCache.length);
        audioRecord.startRecording();

        isRecording = true;
        while (isRecording) {
            audioRecord.read(pcmCache, 0, pcmCache.length);
            try {
                fileOut.write(pcmCache);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        audioRecord.stop();
        audioRecord.release();
        audioRecord = null;
        pcmCache = null;
        try {
            fileOut.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void playPCM() {
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "//test_pcm.pcm");
        FileInputStream fileIn = null;
        try {
            fileIn = new FileInputStream(file);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        //获取符合系统要求的最小缓冲区
        pcmCache = new byte[AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat)];
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz, channelConfig, audioFormat, pcmCache.length, AudioTrack.MODE_STREAM);
        audioTrack.play();

        int byteread = 0;
        while (true) {
            try {
                byteread = fileIn.read(pcmCache);
                if (byteread == -1)
                    break;
            } catch (Exception e) {
                break;
            }
            audioTrack.write(pcmCache, 0, pcmCache.length);
        }

        audioTrack.stop();
        audioTrack.release();
        audioTrack = null;
        pcmCache = null;
        try {
            fileIn.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}




















