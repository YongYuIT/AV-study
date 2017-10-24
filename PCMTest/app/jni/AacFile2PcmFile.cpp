#include"com_thinking_pcmtest_AAC2PCMTools.h"
#include"tools.h"

static const int FRAME_MAX_LEN = 1024 * 5;
static const int ADTS_HEAD = 10;

JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_AacFileToPcmFile
(JNIEnv * env, jclass j_class, jstring aac_file_path)
{
	//读取aac文件路径-------------------------------------------------------------------------------
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "aac test success");
	jstring result;
	string aac_path = env->GetStringUTFChars(aac_file_path, 0);
	//读取首帧数据-------------------------------------------------------------------------------
	ifstream infile(aac_path.c_str(), ifstream::binary);
	unsigned char first_frame[FRAME_MAX_LEN] = { 0 };
	infile.read((char*)first_frame, FRAME_MAX_LEN);
	//获取首帧aac数据实际帧长-------------------------------------------------------------------------------
	size_t frame_size = 0;
	frame_size |= ((first_frame[3] & 0x03) << 11);     //high 2 bit
	frame_size |= first_frame[4] << 3;                 //middle 8 bit
	frame_size |= ((first_frame[5] & 0xe0) >> 5);      //low 3bit
	//检测解码器能力-------------------------------------------------------------------------------
	unsigned long cap = NeAACDecGetCapabilities();
	if (cap == NULL){
		result = env->NewStringUTF("NeAACDecGetCapabilities error");
		return result;
	}
	else{
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->%d", cap);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->LC_DEC_CAP-->%i", (((unsigned long)LC_DEC_CAP)&cap) > 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->MAIN_DEC_CAP-->%i", (((unsigned long)MAIN_DEC_CAP)&cap) > 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->LTP_DEC_CAP-->%i", (((unsigned long)LTP_DEC_CAP)&cap) > 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->LD_DEC_CAP-->%i", (((unsigned long)LD_DEC_CAP)&cap) > 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->ERROR_RESILIENCE_CAP-->%i", (((unsigned long)ERROR_RESILIENCE_CAP)&cap) > 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "decoder cap-->FIXED_POINT_CAP-->%i", (((unsigned long)FIXED_POINT_CAP)&cap) > 0);
	}
	//打开解码器-------------------------------------------------------------------------------
	NeAACDecHandle decoder = NeAACDecOpen();
	if (!decoder){
		result = env->NewStringUTF("NeAACDecOpen error");
		return result;
	}
	//配置解码器参数-------------------------------------------------------------------------------
	NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(decoder);
	if (!conf){
		result = env->NewStringUTF("NeAACDecGetCurrentConfiguration error");
		return result;
	}
	conf->outputFormat = FAAD_FMT_16BIT;
	conf->dontUpSampleImplicitSBR = 1;
	NeAACDecSetConfiguration(decoder, conf);
	//初始化解码器-------------------------------------------------------------------------------
	unsigned long samplerate;//采样率
	unsigned char channels;//音道数
	long res = NeAACDecInit(decoder, first_frame, frame_size, &samplerate, &channels);
	if (res < 0){
		result = env->NewStringUTF("NeAACDecInit error");
		return result;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "first frame data-->%i-->%i", samplerate, channels);
	//释放首帧数据-------------------------------------------------------------------------------
	//循环读取PCM数据-------------------------------------------------------------------------------
	infile.seekg(0, ios::beg);
	long index = 0;
	while (true)
	{
		unsigned char head[ADTS_HEAD] = { 0 };
		infile.read((char*)head, ADTS_HEAD);
		if (infile.eof()){
			break;
		}
		if ((head[0] == 0xff) && ((head[1] & 0xf0) == 0xf0)){
			size_t frame_size = 0;
			frame_size |= ((head[3] & 0x03) << 11);     //high 2 bit
			frame_size |= head[4] << 3;                 //middle 8 bit
			frame_size |= ((head[5] & 0xe0) >> 5);      //low 3bit
			index += frame_size;
			__android_log_print(ANDROID_LOG_INFO, "yuyong adts", "adts-->%i", frame_size);
			//解码
		}
		else{
			index++;
		}
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "move next-->%i", index);
		infile.seekg(index, ios::beg);
	}
	//释放资源-------------------------------------------------------------------------------
	NeAACDecClose(decoder);
	env->ReleaseStringUTFChars(aac_file_path, aac_path.c_str());
	result = env->NewStringUTF("Success!");
	return result;
}

