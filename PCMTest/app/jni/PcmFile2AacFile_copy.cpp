#include"com_thinking_pcmtest_PCM2AACTools.h"
#include <faac.h>
#include<android\log.h>

#include<iostream>
#include<fstream>
using namespace std;

typedef unsigned long   ULONG;
typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef char            _TCHAR;

JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFile
(JNIEnv *env, jclass jClass, jstring pcm_file_path){

	jstring result;

	string pcm_path = env->GetStringUTFChars(pcm_file_path, 0);

	ULONG nSampleRate = 44100;  // 采样率
	UINT nChannels = 2;         // 声道数
	UINT nPCMBitSize = 16;      // 单样本位数
	ULONG nInputSamples = 0;
	ULONG nMaxOutputBytes = 0;

	int nRet;
	faacEncHandle hEncoder;
	faacEncConfigurationPtr pConfiguration;

	int nBytesRead;
	int nPCMBufferSize;
	BYTE* pbPCMBuffer;
	BYTE* pbAACBuffer;

	FILE* fpIn; // PCM file for input
	FILE* fpOut; // AAC file for output

	fpIn = fopen(pcm_path.c_str(), "rb");
	fpOut = fopen((pcm_path + ".aac").c_str(), "wb");

	// (1) Open FAAC engine
	hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
	if (hEncoder == NULL)
	{
		result = env->NewStringUTF("error:faacEncOpen error");
		return result;
	}

	nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
	pbPCMBuffer = new BYTE[nPCMBufferSize];
	pbAACBuffer = new BYTE[nMaxOutputBytes];

	// (2.1) Get current encoding configuration
	pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;

	// (2.2) Set encoding configuration
	nRet = faacEncSetConfiguration(hEncoder, pConfiguration);

	for (int i = 0; 1; i++)
	{
		// 读入的实际字节数，最大不会超过nPCMBufferSize，一般只有读到文件尾时才不是这个值
		nBytesRead = fread(pbPCMBuffer, 1, nPCMBufferSize, fpIn);

		// 输入样本数，用实际读入字节数计算，一般只有读到文件尾时才不是nPCMBufferSize/(nPCMBitSize/8);
		nInputSamples = nBytesRead / (nPCMBitSize / 8);

		// (3) Encode
		nRet = faacEncEncode(hEncoder, (int*)pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "pcm2aac-->%i-->%i", nBytesRead, nInputSamples);
		fwrite(pbAACBuffer, 1, nRet, fpOut);

		if (nBytesRead <= 0)
		{
			break;
		}
	}

	/*
	while(1)
	{
	// (3) Flushing
	nRet = faacEncEncode(
	hEncoder, (int*) pbPCMBuffer, 0, pbAACBuffer, nMaxOutputBytes);

	if(nRet <= 0)
	{
	break;
	}
	}
	*/

	// (4) Close FAAC engine
	nRet = faacEncClose(hEncoder);

	delete[] pbPCMBuffer;
	delete[] pbAACBuffer;
	fclose(fpIn);
	fclose(fpOut);

	//getchar();

	result = env->NewStringUTF((pcm_path + ".aac").c_str());

	env->ReleaseStringUTFChars(pcm_file_path, pcm_path.c_str());
	return result;
}