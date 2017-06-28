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
}

//javah -d G:\WebRtc\20170627001\A-Video\Android-Proj\X264Test\app\jni -classpath G:\WebRtc\20170627001\A-Video\Android-Proj\X264Test\app\build\intermediates\classes\debug com.hsdi.x264test.X264Test
