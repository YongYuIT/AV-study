/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include<faad.h>
#include<android\log.h>
#include<iostream>
#include<fstream>
using namespace std;
/* Header for class com_thinking_pcmtest_AAC2PCMTools */

#ifndef _Included_com_thinking_pcmtest_AAC2PCMTools
#define _Included_com_thinking_pcmtest_AAC2PCMTools
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     com_thinking_pcmtest_AAC2PCMTools
	 * Method:    test
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_test
		(JNIEnv *, jclass);

	/*
	 * Class:     com_thinking_pcmtest_AAC2PCMTools
	 * Method:    AacFileToPcmFile
	 * Signature: (Ljava/lang/String;)Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_AacFileToPcmFile
		(JNIEnv *, jclass, jstring);
#ifdef __cplusplus
}
#endif
#endif