package com.thinking.pcmtest;

/**
 * Created by Yu Yong on 2017/10/12.
 */

public class PCM2AACTools {
    static {
        System.loadLibrary("faac");
        System.loadLibrary("com_thinking_faac");
    }

    public static native void test();
}

//javah -d E:\AV-study\20171012001\AV-study\PCMTest\app\jni -classpath E:\AV-study\20171012001\AV-study\PCMTest\app\build\intermediates\classes\debug com.thinking.pcmtest.PCM2AACTools