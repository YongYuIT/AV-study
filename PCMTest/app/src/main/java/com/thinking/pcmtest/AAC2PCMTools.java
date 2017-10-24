package com.thinking.pcmtest;

/**
 * Created by Yu Yong on 2017/10/16.
 */

public class AAC2PCMTools {

    static {
        System.loadLibrary("faad2");
        System.loadLibrary("com_thinking_faad");
    }

    public static native void test();

    public static native String AacFileToPcmFile(String pcm_file);
}

//javah -d E:\AV-study\20171012001\AV-study\PCMTest\app\jni -classpath E:\AV-study\20171012001\AV-study\PCMTest\app\build\intermediates\classes\debug com.thinking.pcmtest.AAC2PCMTools
