#include"com_thinking_pcmtest_PCM2AACTools.h"
#include <faac.h>
#include<android\log.h>

#include<iostream>
#include<fstream>
using namespace std;

const static int sampleRateInHz = 44100;
const static int channelConfig = 2;
const static int audioFormat = 16;

static unsigned long aacOutPutCacheSize = 0;
static int pcmCacheSize = -1;
static faacEncHandle hEncoder;
static  string pcm_path;
static unsigned char * pcmCache = NULL;
static unsigned char * aacCache = NULL;
static ifstream * infile = NULL;

JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFileOut
(JNIEnv * env, jclass j_class, jstring pcm_file_path){

	//如果编码器没有初始化，表明是首次调用，初始化
	if (hEncoder == NULL){

		//获取PCM文件路径
		pcm_path = env->GetStringUTFChars(pcm_file_path, 0);
		__android_log_print(ANDROID_LOG_INFO, "yuyong_send_from_web", "get pcm file path-->%s", pcm_path.c_str());
		//根据采样率和通道数计算输入/输出缓冲区大小
		unsigned long pcmInputCacheSize = 0;
		hEncoder = faacEncOpen(sampleRateInHz, channelConfig, &pcmInputCacheSize, &aacOutPutCacheSize);
		if (hEncoder == NULL){
			return -1;
		}
		//计算按字节计算的输入缓冲区大小
		pcmCacheSize = pcmInputCacheSize * audioFormat / 8;
		__android_log_print(ANDROID_LOG_INFO, "yuyong_send_from_web", "pcmCacheSize-->%i", pcmCacheSize);
		//初始化输入/输出缓冲区
		pcmCache = new unsigned char[pcmCacheSize];
		aacCache = new unsigned char[aacOutPutCacheSize];
		//填充编码器参数
		faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
		pConfiguration->inputFormat = FAAC_INPUT_16BIT;
		pConfiguration->outputFormat = 1;/* Bitstream output format (0 = Raw; 1 = ADTS) */
		faacEncSetConfiguration(hEncoder, pConfiguration);
		//打开文件流
		infile = new ifstream(pcm_path.c_str(), ifstream::binary);
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong_send_from_web", "start encode");
	//读取PCM并编码
	infile->read((char*)pcmCache, pcmCacheSize);
	int count = infile->gcount() / (audioFormat / 8);
	int nRet = faacEncEncode(hEncoder, (int*)pcmCache, count, aacCache, aacOutPutCacheSize);
	//回调
	jfieldID _mCache_id = env->GetStaticFieldID(j_class, "mCache", "[B");
	jobject _mCache = env->GetStaticObjectField(j_class, _mCache_id);
	unsigned char * c_mCache = (unsigned char *)env->GetByteArrayElements((jbyteArray)_mCache, 0);
	memcpy(c_mCache, aacCache, nRet);
	//循环读取
	if (infile->eof()){
		__android_log_print(ANDROID_LOG_INFO, "yuyong_send_from_web", "loop end========================================================");
		infile->clear();
		infile->seekg(0, ios::beg);
	}
	//返回
	env->ReleaseByteArrayElements((jbyteArray)_mCache, (jbyte*)c_mCache, JNI_COMMIT);
	return nRet;

}

JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFileOutStop
(JNIEnv *, jclass){
	faacEncClose(hEncoder);
	delete[] pcmCache;
	delete[] aacCache;
	infile->close();
}