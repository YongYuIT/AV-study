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

    public static byte[] getmCache() {
        return mCache;
    }

    private static byte[] mCache = new byte[1024];

    public static native int GetAACFrame(String pcm_file);

    public static native int GetAACFrameStop();
}

//javah -d E:\AV-study\20171103001\AV-study\PCMTest\app\jni -classpath E:\AV-study\20171103001\AV-study\PCMTest\app\build\intermediates\classes\debug com.thinking.pcmtest.AAC2PCMTools
