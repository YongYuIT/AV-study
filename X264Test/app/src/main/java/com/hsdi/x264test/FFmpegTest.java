package com.hsdi.x264test;

/**
 * Created by Yu Yong on 2017/7/4.
 */

public class FFmpegTest {
    public static final native boolean ffpmeg_pack_h264_2_mp4(String file_path, String out_put_file_path);
}

//javah -d E:\AV-study\20170704002\AV-study\X264Test\app\jni -classpath E:\AV-study\20170704002\AV-study\X264Test\app\build\intermediates\classes\debug com.hsdi.x264test.FFmpegTest
