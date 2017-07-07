package com.hsdi.x264test;

/**
 * Created by Yu Yong on 2017/7/4.
 */

public class FFmpegTest {
    static {
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("com_thinking_ffmpeg");
    }

    public static final native boolean ffmpeg_test_AV_package(String file_path, String out_put_file_path);

    public static final native boolean ffmpeg_test_V_package(String file_path, String out_put_file_path);
}

//javah -d E:\AV-study\20170706001\AV-study\X264Test\app\jni -classpath E:\AV-study\20170706001\AV-study\X264Test\app\build\intermediates\classes\debug com.hsdi.x264test.FFmpegTest
//adb pull /sdcard/video_test.mp4 .