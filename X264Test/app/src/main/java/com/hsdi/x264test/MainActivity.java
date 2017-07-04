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
import java.io.RandomAccessFile;

public class MainActivity extends Activity {
    //相机方向描述是横向的
    private static final int width = 720;
    private static final int height = 480;
    private static final File video_file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "video_test.h264");

    //SurfaceView和TextureView都是直接继承自View的绘图基本类
    //SurfaceView使用双缓冲，需要频繁重绘画布的使用
    //private SurfaceView suf_view;
    //相机视频类应用使用TextureView
    private TextureView txt_view;
    private Camera mCamera = null;
    private static byte[] mInPut;
    private static byte[] mOutPut;

    private RandomAccessFile WFile;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        X264Test.CppTest();
        txt_view = (TextureView) findViewById(R.id.txt_view);
        txt_view.setSurfaceTextureListener(mTxtViewListener);
        prepareFile();

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
            WFile = new RandomAccessFile(video_file, "rw");
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
                onFeame(data, camera);
                camera.addCallbackBuffer(data);
            }
        }
    };

    private void onViewReady(SurfaceTexture suf) {
        X264Test.x264_test_init(width, height);
        try {
            mCamera = Camera.open(1);
        } catch (Exception e) {
            mCamera = null;
            return;
        }
        try {
            //1、相机参数设定
            Camera.Parameters c_params = mCamera.getParameters();
            c_params.setPreviewSize(width, height);
            c_params.setPictureSize(width, height);
            /*
            I420: YYYYYYYY  UUVV    =>  YUV420P
            YV12: YYYYYYYY  VVUU    =>  YUV420P
            NV12: YYYYYYYY  UVUV    =>  YUV420SP
            NV21: YYYYYYYY  VUVU    =>  YUV420SP
             */
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
            mOutPut = new byte[mInPut.length * 4];
            mCamera.addCallbackBuffer(mInPut);
            mCamera.setPreviewCallbackWithBuffer(mCameraCallBack);
            mCamera.setPreviewTexture(suf);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        mCamera.startPreview();
    }


    private void onFeame(byte[] data, Camera camera) {
        int length = X264Test.x264_test_encode(-1, data, mOutPut);
        try {
            WFile.write(mOutPut, 0, length);
        } catch (Exception e) {
            Log.i("yuyong_p", "x264_test_encode write file failed --> " + e.getMessage());
        }
        Log.i("yuyong_p", "x264_test_encode " + length + " --> " + (length * 100f) / (mOutPut.length + 0f));
    }

    private void OnViewStop() {
        X264Test.x264_test_finish();
        if (mCamera != null) {
            Camera tmp = mCamera;
            mCamera = null;
            tmp.stopPreview();
            tmp.release();
        }
    }
}
