package com.thinking.pcmtest.network;

import android.content.Context;

/**
 * Created by Yu Yong on 2017/11/10.
 */

public abstract class Sender {
    public abstract void init() throws Exception;

    public abstract void destroy();

    public abstract String getIP(Context ctx);

    public abstract void sendMsg(byte[] data, int size);
}
