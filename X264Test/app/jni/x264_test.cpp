#include"com_hsdi_x264test_X264Test.h"
#include<iostream>
#include<android\log.h>
#include"Encoder.h"
using namespace std;

static int width;
static int height;

Encoder en;

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1init
(JNIEnv *, jclass, jint _width, jint _height){
	width = _width;
	height = _height;
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "init start %i , %i", width, height);
	//设置编码器参数
	x264_param_default(&en.params);
	en.params.i_log_level = X264_LOG_NONE;
	en.params.i_width = width;
	en.params.i_height = height;
	en.params.rc.i_lookahead = 0;
	en.params.i_bframe = 0;
	en.params.i_fps_num = 5;
	en.params.i_fps_den = 1;
	//寻找可用编码器
	if ((en.handler = x264_encoder_open(&en.params)) == NULL){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "init fail");
		return;
	}
	//创建帧数据
	x264_picture_alloc(&en.frame, X264_CSP_I420, en.params.i_width, en.params.i_height);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "init success %i --> %i", en.params.i_width, en.params.i_height);
	return;
}

JNIEXPORT jint JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1encode
(JNIEnv * env, jclass j_class, jint type, jbyteArray input, jbyteArray output){

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "start encode %i --> %i", en.params.i_width, en.params.i_height);

	jbyte * input_datas = (jbyte*)env->GetByteArrayElements(input, 0);
	jbyte * output_datas = (jbyte*)env->GetByteArrayElements(output, 0);
	int nPicSize = en.params.i_width*en.params.i_height;

	jbyte * y = (jbyte*)en.frame.img.plane[0];
	jbyte * v = (jbyte*)en.frame.img.plane[1];
	jbyte * u = (jbyte*)en.frame.img.plane[2];
	memcpy(y, input_datas, nPicSize);
	for (int i = 0; i < nPicSize / 4; i++)
	{
		*(u + i) = *(input_datas + nPicSize + i * 2);
		*(v + i) = *(input_datas + nPicSize + i * 2 + 1);
	}

	switch (type)
	{
	case 0:
		en.frame.i_type = X264_TYPE_P;
		break;
	case 1:
		en.frame.i_type = X264_TYPE_IDR;
		break;
	case 2:
		en.frame.i_type = X264_TYPE_I;
		break;
	default:
		en.frame.i_type = X264_TYPE_AUTO;
		break;
	}

	int nNal = -1;
	x264_picture_t pic_out;

	//x264_encoder_encode 返回 负数 编码失败
	//x264_encoder_encode 返回 0 编码成功，但是数据被缓存在pic_out里面
	//x264_encoder_encode 返回 0 编码成功
	if (x264_encoder_encode(en.handler, &en.nal, &nNal, &en.frame, &pic_out) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode error");
		return -1;
	}

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode result %i %i", nNal);

	int result = 0;
	unsigned char * pTmpOut = (unsigned char *)output_datas;
	for (int i = 0; i < nNal; i++){
		memcpy(pTmpOut, (en.nal)[i].p_payload, (en.nal)[i].i_payload);
		pTmpOut += (en.nal)[i].i_payload;
		result += (en.nal)[i].i_payload;
	}
	env->ReleaseByteArrayElements(input, input_datas, 0);
	env->ReleaseByteArrayElements(output, output_datas, 0);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode success %i", result);
	return result;
}

JNIEXPORT jint JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1finish
(JNIEnv *, jclass){
	x264_picture_clean(&en.frame);
	x264_encoder_close(en.handler);
	return 0;
}