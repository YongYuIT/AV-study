#include"com_thinking_pcmtest_AAC2PCMTools.h"
#include"tools.h"


static const  int BUFFER_MAX_LEN = 1024 * 1024;
static const int FRAME_MAX_LEN = 1024 * 5;

JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_AacFileToPcmFile
(JNIEnv * env, jclass j_class, jstring aac_file_path)
{
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "aac test success");
	jstring result;
	string aac_path = env->GetStringUTFChars(aac_file_path, 0);
	//读取首帧数据
	unsigned char buffer[BUFFER_MAX_LEN] = { 0 };
	unsigned char  frame[FRAME_MAX_LEN];
	ifstream infile(aac_path.c_str(), ifstream::binary);
	infile.read((char*)buffer, BUFFER_MAX_LEN);
	int count = infile.gcount();
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "read first data-->%i-->%i-->%i", count, buffer[0] + 0, buffer[1] + 0);
	//解析首帧数据头部
	size_t frame_size;
	if (get_ADTS_frame(buffer, count, frame, &frame_size) < 0){
		result = env->NewStringUTF("get adts frame failed");
		return result;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "get adts success");
	unsigned long samplerate;//采样率
	unsigned char channels;//音道数
	NeAACDecHandle decoder = NeAACDecOpen();
	NeAACDecInit(decoder, frame, frame_size, &samplerate, &channels);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "first frame data-->%i-->%i", samplerate, channels);
	//解码得到PCM数据
	unsigned char* p_bufffer = buffer;
	unsigned char* pcm_data = NULL;
	NeAACDecFrameInfo frame_info;
	ofstream outfile((aac_path + ".pcm").c_str(), ifstream::binary | ios::trunc);
	while (get_ADTS_frame(p_bufffer, count, frame, &frame_size) == 0)
	{
		pcm_data = (unsigned char*)NeAACDecDecode(decoder, &frame_info, frame, frame_size);
		if (frame_info.error > 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "decode failed");
			result = env->NewStringUTF("decode failed");
			return result;
		}
		else if (pcm_data && frame_info.samples > 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "decode-->%i-->%i", frame_info.samples, frame_info.channels);
			int length = frame_info.samples * frame_info.channels;
			outfile.write((char*)pcm_data, length);
		}
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "decode finish");
	NeAACDecClose(decoder);

	infile.close();
	outfile.close();

	env->ReleaseStringUTFChars(aac_file_path, aac_path.c_str());
	return result;
}

