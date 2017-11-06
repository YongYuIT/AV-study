package com.thinking.pcmtest;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by Yu Yong on 2017/10/25.
 */

public class NetworkSender {
    private static NetworkSender thiz;
    private static ExecutorService pool = Executors.newSingleThreadExecutor();

    private NetworkSender() {
    }

    public static NetworkSender getThiz() {
        if (thiz == null)
            thiz = new NetworkSender();
        return thiz;
    }

    private ServerSocket mServer;
    private boolean isKeepAccp = false;
    private List<Socket> mClients = new ArrayList<>();
    private Map<Socket, DataOutputStream> mClientOut = new HashMap<>();

    public void init() throws Exception {
        if (isKeepAccp)
            return;
        mServer = new ServerSocket(9999);
        isKeepAccp = true;
        pool.submit(new Runnable() {
            @Override
            public void run() {
                while (isKeepAccp) {
                    try {
                        Log.i("yuyong_network", "try accept");
                        Socket client = mServer.accept();
                        mClients.add(client);
                        mClientOut.put(client, new DataOutputStream(client.getOutputStream()));
                    } catch (Exception e) {
                        Log.i("yuyong_network", "accept error-->" + e.getMessage());
                    }
                }
            }
        });

    }


    public void sendMsg(byte[] data, int size) {
        String testLog = "";
        for (int i = 0; i < 20; i++) {
            testLog += Integer.toHexString(data[i] & 0xff) + " ";
        }
        Log.i("yuyong_send_from_web", "send adts-->" + size + "-->" + testLog);
        for (int i = 0; i < mClients.size(); i++) {
            try {
                mClientOut.get(mClients.get(i)).write(data, 0, size);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void destroy() {
        isKeepAccp = false;
        for (int i = 0; i < mClients.size(); i++) {
            try {
                mClientOut.get(mClients.get(i)).close();
                mClients.get(i).close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public static String getIP(Context ctx) {
        NetworkInfo info = ((ConnectivityManager) ctx.getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
        if (info.getType() == ConnectivityManager.TYPE_WIFI) {//当前使用无线网络
            WifiManager wifiManager = (WifiManager) ctx.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            String ipAddress = intIP2StringIP(wifiInfo.getIpAddress());//得到IPV4地址
            return ipAddress;
        }
        return "UNKNOWN";
    }

    private static String intIP2StringIP(int ip) {
        return (ip & 0xFF) + "." + ((ip >> 8) & 0xFF) + "." + ((ip >> 16) & 0xFF) + "." + (ip >> 24 & 0xFF);
    }

}
