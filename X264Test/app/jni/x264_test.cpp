#include"com_hsdi_x264test_X264Test.h"
#include<iostream>
#include<fstream>
#include<android\log.h>
#include"Encoder.h"
#include<stdio.h>

#include<sys\time.h>

using namespace std;

static int width;
static int height;

Encoder en;
ofstream file_out;

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1init
(JNIEnv *, jclass, jint _width, jint _height, jint _fps){
	width = _width;
	height = _height;
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "init start %i , %i", width, height);
	//设置编码器参数-------------------------start
	if (1 == 0){
		//自己探索
		x264_param_default(&en.params);
		en.params.i_log_level = X264_LOG_NONE;
		en.params.i_width = width;
		en.params.i_height = height;
		//下面这两个参数决定帧率，fps=i_fps_num/i_fps_den
		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;
		//常见的码率控制模式有三种：ABR（平均码率）、CQP（恒定质量）、CRF（恒定码率），默认的方法是CRF
		//这里尝试恒定码率
		en.params.rc.i_rc_method = X264_RC_CRF;
		en.params.rc.f_rf_constant = 40;
		en.params.rc.f_rf_constant_max = 50;
		//多线程编码
		//i_threads=0，根据CUP情况动态决定线程数
		//i_threads=1，明确指定只用单线程编码
		//i_threads=N，使用N个线程并行编码
		if (1 == 1){
			//无帧延时
			en.params.i_threads = 0;
			en.params.rc.i_lookahead = 0;//i_lookahead = 0，不缓存帧，编码一帧输出一帧
		}
		else if (1 == 0){
			//无帧延时
			en.params.i_threads = 8;
			en.params.b_sliced_threads = 1;//8个slice并行编码
			en.params.rc.i_lookahead = 0;//i_lookahead = 0，不缓存帧，编码一帧输出一帧
		}
		else if (1 == 0){
			//有帧延时
			en.params.i_threads = 8;
			en.params.b_sliced_threads = 0;//8个frame并行编码
			en.params.rc.i_lookahead = 8;//缓存max(i_threads, rc.i_lookahead)帧
		}
		else{
			en.params.i_threads = X264_THREADS_AUTO; /* 并行编码线程为0 */
			en.params.b_deterministic = 1; /*允许非确定性时线程优化*/
			en.params.i_sync_lookahead = X264_SYNC_LOOKAHEAD_AUTO;/* 自动选择线程超前缓冲大小-1 */
		}
		en.params.i_bframe = 0;//最大B帧数（B帧-双向预测帧拥有最大压缩比，但是不适用于实时编码）
	}
	else if (1 == 0){
		//网上推荐
		x264_param_default_preset(&en.params, "fast", "zerolatency");
		en.params.i_width = width;
		en.params.i_height = height;
		en.params.b_repeat_headers = 1;  // 重复SPS/PPS 放到关键帧前面          
		en.params.b_cabac = 1;
		en.params.i_threads = 1;
		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;
		en.params.i_keyint_max = _fps * 2;
		//rc
		//en.params.rc.b_mb_tree=0;//找个不为0,将导致编码延时帧...在实时编码时,必须为0
		en.params.rc.f_rf_constant = 25;
		en.params.rc.f_rf_constant_max = 45;
		en.params.rc.i_rc_method = X264_RC_ABR;//参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
		//en.params.rc.f_rate_tolerance=0.1;
		en.params.rc.i_vbv_max_bitrate = (int)((_fps*1.2) / 1000); // 平均码率模式下，最大瞬时码率，默认0(与-B设置相同)
		en.params.rc.i_bitrate = (int)_fps / 1000;
		x264_param_apply_profile(&en.params, "baseline");
		en.params.i_level_idc = 30;
		en.params.i_log_level = X264_LOG_NONE;
	}
	else{
		//自己改进
		x264_param_default_preset(&en.params, "fast", "zerolatency");
		en.params.i_width = width;
		en.params.i_height = height;

		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;

		en.params.rc.i_rc_method = X264_RC_CRF;
		en.params.rc.f_rf_constant = 30;
		en.params.rc.f_rf_constant_max = 50;
	}

	//设置编码器参数-------------------------end
	//尝试打开编码器
	if ((en.handler = x264_encoder_open(&en.params)) == NULL){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "init fail");
		return;
	}
	//创建帧数据载体
	x264_picture_alloc(&en.frame, X264_CSP_I420, en.params.i_width, en.params.i_height);

	//打开文件流（这个与纯Java写文件方式冲突，如果需要纯Java写文件，这步操作要注释）
	//out:为输出而打开；app:文件尾追加；binary:二进制
	file_out.open("/sdcard/video_test.h264", ios::out | ios::app | ios::binary);

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "init success %i --> %i", en.params.i_width, en.params.i_height);
	return;
}

JNIEXPORT jint JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1encode__I_3B_3B
(JNIEnv * env, jclass j_class, jint type, jbyteArray input, jbyteArray output){

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "start encode %i --> %i", en.params.i_width, en.params.i_height);

	jbyte * input_datas = (jbyte*)env->GetByteArrayElements(input, 0);
	jbyte * output_datas = (jbyte*)env->GetByteArrayElements(output, 0);

	//数据格式转换
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

	//指定数据帧
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

	//编码
	int nNal = -1;
	x264_picture_t pic_out;
	/*
	x264_encoder_encode 返回 负数 编码失败
	x264_encoder_encode 返回 0 编码成功，但是数据被缓存在pic_out里面
	x264_encoder_encode 返回 0 编码成功
	*/
	if (x264_encoder_encode(en.handler, &en.nal, &nNal, &en.frame, &pic_out) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode error");
		return -1;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode result %i %i", nNal);
	//数据输出
	int result = 0;
	unsigned char * pTmpOut = (unsigned char *)output_datas;
	for (int i = 0; i < nNal; i++){
		memcpy(pTmpOut, (en.nal)[i].p_payload, (en.nal)[i].i_payload);
		pTmpOut += (en.nal)[i].i_payload;
		result += (en.nal)[i].i_payload;
	}

	//数据释放
	env->ReleaseByteArrayElements(input, input_datas, 0);
	env->ReleaseByteArrayElements(output, output_datas, 0);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode success %i", result);
	return result;
}

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1encode__I_3B
(JNIEnv * env, jclass j_class, jint type, jbyteArray input){




	__android_log_print(ANDROID_LOG_INFO, "yuyong", "start encode %i --> %i", en.params.i_width, en.params.i_height);

	jbyte * input_datas = (jbyte*)env->GetByteArrayElements(input, 0);

	//数据格式转换
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

	//指定数据帧
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

	//编码
	int nNal = -1;
	x264_picture_t pic_out;
	/*
	x264_encoder_encode 返回 负数 编码失败
	x264_encoder_encode 返回 0    编码成功，但是数据被缓存在pic_out里面
	x264_encoder_encode 返回 0    编码成功
	*/
	timeval time_before_encode;
	timeval time_after_encode;
	gettimeofday(&time_before_encode, NULL);
	if (x264_encoder_encode(en.handler, &en.nal, &nNal, &en.frame, &pic_out) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode error");
		return;
	}
	gettimeofday(&time_after_encode, NULL);
	//tv_sec：秒
	//tv_usec：微秒，10的负6次方秒，千分之一毫秒
	__android_log_print(ANDROID_LOG_ERROR, "yuyong_encode", "encode result %i-->%ld", nNal, (time_after_encode.tv_sec - time_before_encode.tv_sec) * 1000 * 1000 + time_after_encode.tv_usec - time_before_encode.tv_usec);

	//数据输出
	//x264_nal_t结构体用于存储x264_encoder_encode函数编码完成之后的数据
	/*
	typedef struct x264_nal_t
	{
	int i_ref_idc;
	int i_type;
	int b_long_startcode;
	int i_first_mb;
	int i_last_mb;
	int i_payload;     //编码数据域
	uint8_t *p_payload;//编码数据长度
	int i_padding;
	} x264_nal_t;
	*/
	unsigned char * out_put = (unsigned char *)malloc(sizeof(jbyte)*env->GetArrayLength(input));
	unsigned char *pTmpOut = out_put;
	int result = 0;
	for (int i = 0; i < nNal; i++){
		memcpy(pTmpOut, (en.nal)[i].p_payload, (en.nal)[i].i_payload);
		pTmpOut += (en.nal)[i].i_payload;
		result += (en.nal)[i].i_payload;
	}
	pTmpOut = NULL;
	gettimeofday(&time_before_encode, NULL);
	file_out.write((char *)out_put, sizeof(unsigned char)*result);
	gettimeofday(&time_after_encode, NULL);
	__android_log_print(ANDROID_LOG_ERROR, "yuyong", "save data %i-->%ld", result, (time_after_encode.tv_sec - time_before_encode.tv_sec) * 1000 * 1000 + time_after_encode.tv_usec - time_before_encode.tv_usec);


	//数据释放
	env->ReleaseByteArrayElements(input, input_datas, 0);
	free(out_put);
	out_put = NULL;
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode success %i", result);
	return;
}

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1finish
(JNIEnv *, jclass){
	x264_picture_clean(&en.frame);
	x264_encoder_close(en.handler);
	//（这个与纯Java写文件方式冲突，如果需要纯Java写文件，这步操作要注释）
	file_out.close();
	return;
}