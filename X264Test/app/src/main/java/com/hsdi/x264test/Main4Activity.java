package com.hsdi.x264test;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.TextureView;
import android.view.ViewGroup;

import java.io.File;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Date;
import java.util.List;
import java.util.Stack;

//C/CPP编码，C/CPP保存数据，有YUV帧缓存
public class Main4Activity extends Activity {
    //相机方向描述是横向的
    private static final int width = 800;
    private static final int height = 480;
    private static final File video_file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264");

    //帧率控制相关参数
    private final static int MAX_FPS = 10;
    private final static int FRAME_PERIOD = (1000 / MAX_FPS);
    long lastTime = 0;
    long timeDiff = 0;

    //SurfaceView和TextureView都是直接继承自View的绘图基本类
    //SurfaceView使用双缓冲，需要频繁重绘画布的使用
    //private SurfaceView suf_view;
    //相机视频类应用使用TextureView
    private TextureView txt_view;
    private Camera mCamera = null;

    private static byte[] mInPut;
    //缓冲数据域
    private static Stack<byte[]> mFrameCache = new Stack<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        X264Test.CppTest();
        txt_view = (TextureView) findViewById(R.id.txt_view);
        txt_view.setSurfaceTextureListener(mTxtViewListener);
        prepareFile();
    }

    @Override
    protected void onDestroy() {
        mFrameCache.clear();
        super.onDestroy();
    }

    private void prepareFile() {
        if (video_file.exists()) {
            File tmp = new File(video_file.getParentFile().getAbsolutePath() + "/fuck");
            video_file.renameTo(tmp);
            tmp.delete();
        }
        if (video_file.getParentFile().exists()) {
            video_file.getParentFile().mkdirs();
        }
        try {
            video_file.createNewFile();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private TextureView.SurfaceTextureListener mTxtViewListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
            onViewReady(surfaceTexture);
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {

        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
            OnViewStop();
            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

        }
    };


    private Camera.PreviewCallback mCameraCallBack = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            if (mCamera != null) {
                camera.addCallbackBuffer(data);
            }
            //控制帧率-------------start
            timeDiff = System.currentTimeMillis() - lastTime;
            if (timeDiff < FRAME_PERIOD) {
                return;
            }
            lastTime = System.currentTimeMillis();
            //控制帧率-------------end
            if (mCamera != null) {
                onFeame(data, camera);
            }
        }
    };

    private void onViewReady(SurfaceTexture suf) {
        X264Test.x264_test_init(width, height, MAX_FPS);
        try {
            mCamera = Camera.open(1);
        } catch (Exception e) {
            mCamera = null;
            return;
        }
        try {
            //1、相机参数设定
            Camera.Parameters c_params = mCamera.getParameters();
            List<Camera.Size> sizes = c_params.getSupportedPreviewSizes();
            c_params.setPreviewSize(width, height);
            c_params.setPictureSize(width, height);
            /*
            I420: YYYYYYYY  UUVV    =>  YUV420P
            YV12: YYYYYYYY  VVUU    =>  YUV420P
            NV12: YYYYYYYY  UVUV    =>  YUV420SP
            NV21: YYYYYYYY  VUVU    =>  YUV420SP
             */
            c_params.setPreviewFormat(ImageFormat.NV21);
            mCamera.setDisplayOrientation(270);
            mCamera.setParameters(c_params);
            //2、视图参数设定
            ViewGroup.LayoutParams t_params = txt_view.getLayoutParams();
            t_params.width = height;
            t_params.height = width;
            txt_view.setLayoutParams(t_params);
            //3、缓存数据设定
            mInPut = new byte[mCamera.getParameters().getPreviewSize().width * mCamera.getParameters().getPreviewSize().height * 3 / 2];
            mCamera.addCallbackBuffer(mInPut);
            mCamera.setPreviewCallbackWithBuffer(mCameraCallBack);
            mCamera.setPreviewTexture(suf);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        mEncodeThread.start();
        mCamera.startPreview();
    }

    private void onFeame(byte[] data, Camera camera) {
        byte[] tmp = new byte[data.length];
        System.arraycopy(data, 0, tmp, 0, data.length);
        if (mFrameCache.size() > MAX_FPS * 2) {
            Log.e("yuyong_p", "忙，丢弃");
        } else {
            Log.e("yuyong_p", new Date().getSeconds() + "-->有效帧-->" + (mFrameCache.size() * 100 + 0f) / (MAX_FPS * 2 + 0f) + "%");
            mFrameCache.push(tmp);
        }
        tmp = null;
    }

    private void OnViewStop() {
        mKeepRunning = false;
        X264Test.x264_test_finish();
        if (mCamera != null) {
            Camera tmp = mCamera;
            mCamera = null;
            tmp.stopPreview();
            tmp.release();
        }
    }

    private static boolean mKeepRunning = true;
    private static Thread mEncodeThread = new Thread() {
        @Override
        public void run() {
            while (mKeepRunning) {
                if (!mFrameCache.empty()) {
                    byte[] tmp = mFrameCache.pop();
                    X264Test.x264_test_encode(-1, tmp);
                    tmp = null;
                    System.gc();
                }
            }
        }
    };
}
