package com.hsdi.mediacodectest;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.util.Log;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * Created by Yu Yong on 2017/7/12.
 */

public class Recorder {

    public interface Listener {
        void onEncode(boolean isSuccess, byte[] bytes);
    }

    private static final String H264_TypeVal = "video/avc";
    private static final int TIMEOUT = 10000;//超时时间

    private Listener mListener;
    private byte[] mOutPut;

    private MediaCodec mMediaCodec;
    private boolean isRunning = false;

    public Recorder(int width, int height, int f_rate) {
        //参数设置
        MediaCodecInfo info = selectCodec();
        MediaFormat format = MediaFormat.createVideoFormat(H264_TypeVal, width, height);
        format.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 3 * 8 * f_rate / 256);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, f_rate);
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT, selectColorFormat(info.getCapabilitiesForType(H264_TypeVal)));
        //创建数据输出缓冲
        mOutPut = new byte[width * height * 3 / 2];
        //创建编码器
        try {
            mMediaCodec = MediaCodec.createByCodecName(info.getName());
        } catch (Exception e) {
            mMediaCodec = null;
        }
        mMediaCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        isRunning = false;
    }

    public void addListener(Listener listener) {
        mListener = listener;
    }

    public synchronized void doEncode(byte[] input) {
        if (!isRunning) {
            mMediaCodec.start();
            isRunning = true;
        }

        ByteBuffer[] input_bufs = mMediaCodec.getInputBuffers();
        int input_buf_index = mMediaCodec.dequeueInputBuffer(TIMEOUT);
        if (input_buf_index > 0) {
            ByteBuffer input_buf = input_bufs[input_buf_index];
            input_buf.clear();
            input_buf.put(input);
            mMediaCodec.queueInputBuffer(input_buf_index, 0, input.length, System.nanoTime() / 1000, 0);
        }

        MediaCodec.BufferInfo tmp_info = new MediaCodec.BufferInfo();
        int out_buf_index = -1;
        while ((out_buf_index = mMediaCodec.dequeueOutputBuffer(tmp_info, TIMEOUT)) >= 0) {
            ByteBuffer outputBuffer = mMediaCodec.getOutputBuffers()[out_buf_index];
        }
    }

    private static final MediaCodecInfo selectCodec() {
        int numCodecs = MediaCodecList.getCodecCount();
        Log.i("yuyong", "codec num-->" + numCodecs);
        //遍历，查看当前手机显卡支持那些编码标准
        for (int i = 0; i < numCodecs; i++) {
            MediaCodecInfo codec_info = MediaCodecList.getCodecInfoAt(i);
            Log.i("yuyong", "codec name-->" + codec_info.getName() + "-->" + getStringListVal(codec_info.getSupportedTypes()));
            if (getStringListVal(codec_info.getSupportedTypes()).contains(H264_TypeVal))
                return codec_info;
        }
        return null;
    }

    //为编解码器选择合适的颜色编码
    private static final int selectColorFormat(MediaCodecInfo.CodecCapabilities capabilities) {
        for (int i = 0; i < capabilities.colorFormats.length; i++) {
            int colorFormat = capabilities.colorFormats[i];
            if (isRecognizedFormat(colorFormat)) {
                return colorFormat;
            }
        }
        return -1;
    }

    private static final String getStringListVal(String... list) {
        String rst = "";
        for (int i = 0; i < list.length; i++) {
            rst += list[i] + "##";
        }
        return rst;
    }

    //是否是Camera支持的输出颜色编码
    private static final boolean isRecognizedFormat(int colorFormat) {
        switch (colorFormat) {
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedSemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_TI_FormatYUV420PackedSemiPlanar:
                return true;
            default:
                return false;
        }
    }
}
