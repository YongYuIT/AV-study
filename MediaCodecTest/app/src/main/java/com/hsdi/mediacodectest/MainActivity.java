package com.hsdi.mediacodectest;

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
import java.util.List;

public class MainActivity extends Activity {
    //相机方向描述是横向的
    private static final int width = 1280;
    private static final int height = 720;
    private static final File video_file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264");

    //帧率控制相关参数
    private final static int MAX_FPS = 10;
    private final static int FRAME_PERIOD = (1000 / MAX_FPS);
    long lastTime = 0;
    long timeDiff = 0;

    private TextureView txt_view;
    private Camera mCamera = null;
    private static byte[] mInPut;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txt_view = (TextureView) findViewById(R.id.txt_view);
        txt_view.setSurfaceTextureListener(mTxtViewListener);
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
                Log.i("yuyong_p", "有效帧");
                onFeame(data, camera);
            }
        }
    };

    private void onViewReady(SurfaceTexture suf) {
        try {
            mCamera = Camera.open(0);
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
            c_params.setPreviewFormat(ImageFormat.NV21);
            mCamera.setDisplayOrientation(90);
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

    }

    private void OnViewStop() {
        if (mCamera != null) {
            Camera tmp = mCamera;
            mCamera = null;
            tmp.stopPreview();
            tmp.release();
        }
    }

    private static Thread mEncodeThread = new Thread() {
        @Override
        public void run() {

        }
    };
}
