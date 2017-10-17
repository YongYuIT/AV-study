#include"com_thinking_pcmtest_PCM2AACTools.h"
#include <faac.h>
#include<android\log.h>

#include<iostream>
#include<fstream>
using namespace std;

const static int sampleRateInHz = 44100;
const static int channelConfig = 2;
const static int audioFormat = 16;

JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFile
(JNIEnv *env, jclass jClass, jstring pcm_file_path){

	jstring result;

	string pcm_path = env->GetStringUTFChars(pcm_file_path, 0);

	unsigned long pcmInputCacheSize = 0;
	unsigned long aacOutPutCacheSize = 0;

	faacEncHandle hEncoder = faacEncOpen(sampleRateInHz, channelConfig, &pcmInputCacheSize, &aacOutPutCacheSize);
	if (hEncoder == NULL){
		result = env->NewStringUTF("error:faacEncOpen error");
		return result;
	}

	int pcmCacheSize = pcmInputCacheSize * audioFormat / 8;
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "pcmCacheSize-->%i", pcmCacheSize);

	unsigned char * pcmCache = new unsigned char[pcmCacheSize];
	unsigned char * aacCache = new unsigned char[aacOutPutCacheSize];

	faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;
	pConfiguration->outputFormat = 1;/* Bitstream output format (0 = Raw; 1 = ADTS) */

	ifstream infile(pcm_path.c_str(), ifstream::binary);
	ofstream outfile((pcm_path + ".aac").c_str(), ifstream::binary | ios::trunc);
	int nRet = faacEncSetConfiguration(hEncoder, pConfiguration);
	while (true){
		infile.read((char*)pcmCache, pcmCacheSize);
		int count = infile.gcount() / (audioFormat / 8);
		nRet = faacEncEncode(hEncoder, (int*)pcmCache, count, aacCache, aacOutPutCacheSize);
		outfile.write((char*)aacCache, nRet);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "pcm2aac-->%i-->%i-->%i", infile.gcount(), count, nRet);
		if (infile.eof()){
			break;
		}
	}

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "pcm2aac finish");

	nRet = faacEncClose(hEncoder);
	delete[] pcmCache;
	delete[] aacCache;
	infile.close();
	outfile.close();

	result = env->NewStringUTF((pcm_path + ".aac").c_str());

	env->ReleaseStringUTFChars(pcm_file_path, pcm_path.c_str());
	return result;
}