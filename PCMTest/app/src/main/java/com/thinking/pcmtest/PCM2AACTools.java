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

    public static native String PcmFileToAccFile(String pcm_file);

    public static byte[] getmCache() {
        return mCache;
    }

    private static byte[] mCache = new byte[1024];

    public static native int PcmFileToAccFileOut(String pcm_file);

    public static native int PcmFileToAccFileOutStop();
}

//javah -d E:\AV-study\20171103001\AV-study\PCMTest\app\jni -classpath E:\AV-study\20171103001\AV-study\PCMTest\app\build\intermediates\classes\debug com.thinking.pcmtest.PCM2AACTools