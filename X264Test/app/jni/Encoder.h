#ifndef COM_HSDI_X264_TEST_ENCODER_H
#define COM_HSDI_X264_TEST_ENCODER_H

#include"com_hsdi_x264test_X264Test.h"
#include<iostream>
using namespace std;

//����������
class Encoder
{
public:
	x264_param_t  params;
	x264_t*  handler;
	x264_picture_t frame;
	x264_nal_t nal;
	Encoder(){
		//�޲ι��캯��
		handler = NULL;
		//frame = NULL;
	}
	~Encoder(){
		//�޲���������
		if (handler != NULL)
			free(handler);
		handler = NULL;
	}
};

#endif