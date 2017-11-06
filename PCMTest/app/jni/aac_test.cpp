#include"com_thinking_pcmtest_AAC2PCMTools.h"
#include<android\log.h>
#include<faad.h>
#include"tools.h"

#include<iostream>
#include<fstream>
using namespace std;

static const  int BUFFER_MAX_LEN = 1024 * 1024;
static const int FRAME_MAX_LEN = 1024 * 5;

JNIEXPORT void JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_test
(JNIEnv *, jclass){
	unsigned char buffer[BUFFER_MAX_LEN] = { 0 };
	ifstream infile("/sdcard/test_pcm.pcm.aac", ifstream::binary);
	infile.read((char*)buffer, BUFFER_MAX_LEN);
	int count = infile.gcount();
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "read first data-->%i-->%i-->%i", count, buffer[0] + 0, buffer[1] + 0);
	if ((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0))
	{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "start with aac head");
	}
	size_t frame_size = 0;
	frame_size |= ((buffer[3] & 0x03) << 11);     //high 2 bit
	frame_size |= buffer[4] << 3;                 //middle 8 bit
	frame_size |= ((buffer[5] & 0xe0) >> 5);      //low 3bit
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "aac head size is-->%i", frame_size);
	unsigned char*  frame = (unsigned char*)malloc(sizeof(unsigned char)*frame_size);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "read first data before-->%i-->%i", frame[0] + 0, frame[1] + 0);
	memcpy(frame, buffer, frame_size);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "read first data after-->%i-->%i", frame[0] + 0, frame[1] + 0);
	unsigned long cap = NeAACDecGetCapabilities();
	NeAACDecHandle decoder = NeAACDecOpen();
	if (!decoder){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "create decoder error");
		return;
	}
	NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(decoder);
	if (!conf){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "create conf error");
		return;
	}
	NeAACDecSetConfiguration(decoder, conf);

	unsigned long samplerate;//采样率
	unsigned char channels;//音道数
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "try read first frame data");
	if (1 == 1){
		long res = NeAACDecInit(decoder, frame, frame_size, &samplerate, &channels);
		if (res < 0){
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "init error");
			return;
		}
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "first frame data-->%i-->%i", samplerate, channels);
	}
	else{
		NeAACDecFrameInfo frame_info;
		unsigned char* pcm_data = (unsigned char*)NeAACDecDecode(decoder, &frame_info, frame, frame_size);
		if (frame_info.error > 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "decode failed-->%i", frame_info.error);
		}
		else if (pcm_data && frame_info.samples > 0)
		{
			int pcm_length = frame_info.samples * frame_info.channels;
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "decode-->%i-->%i-->%i", frame_info.samples, frame_info.channels, pcm_length);
		}
	}
	free(frame);
	NeAACDecClose(decoder);
	infile.close();
}