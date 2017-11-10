package com.thinking.pcmtest.network;

import android.content.Context;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by Yu Yong on 2017/11/7.
 */

public class MultiNetworkSender extends Sender {
    private static MultiNetworkSender thiz;
    private static ExecutorService pool = Executors.newSingleThreadExecutor();

    private MultiNetworkSender() {
    }

    public static MultiNetworkSender getThiz() {
        if (thiz == null)
            thiz = new MultiNetworkSender();
        return thiz;
    }

    //局域网内部的组播地址：239.0.0.0~239.255.255.255
    private final static String mutIP = "224.0.0.1";
    private final static int mutIP_port = 3333;
    private MulticastSocket mServer;

    @Override
    public String getIP(Context ctx) {
        return mutIP;
    }

    @Override
    public void init() throws Exception {
        if (mServer != null)
            return;
        mServer = new MulticastSocket(mutIP_port);
        mServer.joinGroup(InetAddress.getByName(mutIP));
    }

    @Override
    public void destroy() {
        try {
            mServer.leaveGroup(InetAddress.getByName(mutIP));
        } catch (Exception e) {
            e.printStackTrace();
        }
        mServer = null;
    }

    @Override
    public void sendMsg(byte[] data, int size) {
        String testLog = "";
        for (int i = 0; i < 20; i++) {
            testLog += Integer.toHexString(data[i] & 0xff) + " ";
        }
        try {
            DatagramPacket dp = new DatagramPacket(data, 0, size, InetAddress.getByName(mutIP), mutIP_port);
            mServer.send(dp);
        } catch (Exception e) {
            e.printStackTrace();
        }
        Log.i("yuyong_send_from_web", "send adts-->" + size + "-->" + testLog);
    }
}
