package com.hsdi.x264test;

/**
 * Created by Yu Yong on 2017/6/27.
 */

public class X264Test {

    static {
        System.loadLibrary("x264-150");
        System.loadLibrary("com_thinking_x264");
    }

    public static final native void CppTest();

    public static final native void x264_test_init(int width, int height);

    public static final native int x264_test_encode(int type, byte[] input, byte[] out);

    public static final native void x264_test_encode(int type, byte[] input);

    public static final native void x264_test_finish();
}

//javah -d E:\AV-study\20170705002\AV-study\X264Test\app\jni -classpath E:\AV-study\20170705002\AV-study\X264Test\app\build\intermediates\classes\debug com.hsdi.x264test.X264Test
