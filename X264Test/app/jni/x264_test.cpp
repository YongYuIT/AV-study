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
	//���ñ���������-------------------------start
	if (1 == 0){
		//�Լ�̽��
		x264_param_default(&en.params);
		en.params.i_log_level = X264_LOG_NONE;
		en.params.i_width = width;
		en.params.i_height = height;
		//������������������֡�ʣ�fps=i_fps_num/i_fps_den
		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;
		//���������ʿ���ģʽ�����֣�ABR��ƽ�����ʣ���CQP���㶨��������CRF���㶨���ʣ���Ĭ�ϵķ�����CRF
		//���ﳢ�Ժ㶨����
		en.params.rc.i_rc_method = X264_RC_CRF;
		en.params.rc.f_rf_constant = 40;
		en.params.rc.f_rf_constant_max = 50;
		//���̱߳���
		//i_threads=0������CUP�����̬�����߳���
		//i_threads=1����ȷָ��ֻ�õ��̱߳���
		//i_threads=N��ʹ��N���̲߳��б���
		if (1 == 1){
			//��֡��ʱ
			en.params.i_threads = 0;
			en.params.rc.i_lookahead = 0;//i_lookahead = 0��������֡������һ֡���һ֡
		}
		else if (1 == 0){
			//��֡��ʱ
			en.params.i_threads = 8;
			en.params.b_sliced_threads = 1;//8��slice���б���
			en.params.rc.i_lookahead = 0;//i_lookahead = 0��������֡������һ֡���һ֡
		}
		else if (1 == 0){
			//��֡��ʱ
			en.params.i_threads = 8;
			en.params.b_sliced_threads = 0;//8��frame���б���
			en.params.rc.i_lookahead = 8;//����max(i_threads, rc.i_lookahead)֡
		}
		else{
			en.params.i_threads = X264_THREADS_AUTO; /* ���б����߳�Ϊ0 */
			en.params.b_deterministic = 1; /*�����ȷ����ʱ�߳��Ż�*/
			en.params.i_sync_lookahead = X264_SYNC_LOOKAHEAD_AUTO;/* �Զ�ѡ���̳߳�ǰ�����С-1 */
		}
		en.params.i_bframe = 0;//���B֡����B֡-˫��Ԥ��֡ӵ�����ѹ���ȣ����ǲ�������ʵʱ���룩
	}
	else if (1 == 0){
		//�����Ƽ�
		x264_param_default_preset(&en.params, "fast", "zerolatency");
		en.params.i_width = width;
		en.params.i_height = height;
		en.params.b_repeat_headers = 1;  // �ظ�SPS/PPS �ŵ��ؼ�֡ǰ��          
		en.params.b_cabac = 1;
		en.params.i_threads = 1;
		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;
		en.params.i_keyint_max = _fps * 2;
		//rc
		//en.params.rc.b_mb_tree=0;//�Ҹ���Ϊ0,�����±�����ʱ֡...��ʵʱ����ʱ,����Ϊ0
		en.params.rc.f_rf_constant = 25;
		en.params.rc.f_rf_constant_max = 45;
		en.params.rc.i_rc_method = X264_RC_ABR;//����i_rc_method��ʾ���ʿ��ƣ�CQP(�㶨����)��CRF(�㶨����)��ABR(ƽ������)
		//en.params.rc.f_rate_tolerance=0.1;
		en.params.rc.i_vbv_max_bitrate = (int)((_fps*1.2) / 1000); // ƽ������ģʽ�£����˲ʱ���ʣ�Ĭ��0(��-B������ͬ)
		en.params.rc.i_bitrate = (int)_fps / 1000;
		x264_param_apply_profile(&en.params, "baseline");
		en.params.i_level_idc = 30;
		en.params.i_log_level = X264_LOG_NONE;
	}
	else{
		//�Լ��Ľ�
		x264_param_default_preset(&en.params, "fast", "zerolatency");
		en.params.i_width = width;
		en.params.i_height = height;

		en.params.i_fps_num = _fps;
		en.params.i_fps_den = 1;

		en.params.rc.i_rc_method = X264_RC_CRF;
		en.params.rc.f_rf_constant = 30;
		en.params.rc.f_rf_constant_max = 50;
	}

	//���ñ���������-------------------------end
	//���Դ򿪱�����
	if ((en.handler = x264_encoder_open(&en.params)) == NULL){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "init fail");
		return;
	}
	//����֡��������
	x264_picture_alloc(&en.frame, X264_CSP_I420, en.params.i_width, en.params.i_height);

	//���ļ���������봿Javaд�ļ���ʽ��ͻ�������Ҫ��Javaд�ļ����ⲽ����Ҫע�ͣ�
	//out:Ϊ������򿪣�app:�ļ�β׷�ӣ�binary:������
	file_out.open("/sdcard/video_test.h264", ios::out | ios::app | ios::binary);

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "init success %i --> %i", en.params.i_width, en.params.i_height);
	return;
}

JNIEXPORT jint JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1encode__I_3B_3B
(JNIEnv * env, jclass j_class, jint type, jbyteArray input, jbyteArray output){

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "start encode %i --> %i", en.params.i_width, en.params.i_height);

	jbyte * input_datas = (jbyte*)env->GetByteArrayElements(input, 0);
	jbyte * output_datas = (jbyte*)env->GetByteArrayElements(output, 0);

	//���ݸ�ʽת��
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

	//ָ������֡
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

	//����
	int nNal = -1;
	x264_picture_t pic_out;
	/*
	x264_encoder_encode ���� ���� ����ʧ��
	x264_encoder_encode ���� 0 ����ɹ����������ݱ�������pic_out����
	x264_encoder_encode ���� 0 ����ɹ�
	*/
	if (x264_encoder_encode(en.handler, &en.nal, &nNal, &en.frame, &pic_out) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode error");
		return -1;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode result %i %i", nNal);
	//�������
	int result = 0;
	unsigned char * pTmpOut = (unsigned char *)output_datas;
	for (int i = 0; i < nNal; i++){
		memcpy(pTmpOut, (en.nal)[i].p_payload, (en.nal)[i].i_payload);
		pTmpOut += (en.nal)[i].i_payload;
		result += (en.nal)[i].i_payload;
	}

	//�����ͷ�
	env->ReleaseByteArrayElements(input, input_datas, 0);
	env->ReleaseByteArrayElements(output, output_datas, 0);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "encode success %i", result);
	return result;
}

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_x264_1test_1encode__I_3B
(JNIEnv * env, jclass j_class, jint type, jbyteArray input){




	__android_log_print(ANDROID_LOG_INFO, "yuyong", "start encode %i --> %i", en.params.i_width, en.params.i_height);

	jbyte * input_datas = (jbyte*)env->GetByteArrayElements(input, 0);

	//���ݸ�ʽת��
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

	//ָ������֡
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

	//����
	int nNal = -1;
	x264_picture_t pic_out;
	/*
	x264_encoder_encode ���� ���� ����ʧ��
	x264_encoder_encode ���� 0    ����ɹ����������ݱ�������pic_out����
	x264_encoder_encode ���� 0    ����ɹ�
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
	//tv_sec����
	//tv_usec��΢�룬10�ĸ�6�η��룬ǧ��֮һ����
	__android_log_print(ANDROID_LOG_ERROR, "yuyong_encode", "encode result %i-->%ld", nNal, (time_after_encode.tv_sec - time_before_encode.tv_sec) * 1000 * 1000 + time_after_encode.tv_usec - time_before_encode.tv_usec);

	//�������
	//x264_nal_t�ṹ�����ڴ洢x264_encoder_encode�����������֮�������
	/*
	typedef struct x264_nal_t
	{
	int i_ref_idc;
	int i_type;
	int b_long_startcode;
	int i_first_mb;
	int i_last_mb;
	int i_payload;     //����������
	uint8_t *p_payload;//�������ݳ���
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


	//�����ͷ�
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
	//������봿Javaд�ļ���ʽ��ͻ�������Ҫ��Javaд�ļ����ⲽ����Ҫע�ͣ�
	file_out.close();
	return;
}