/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_thinking_pcmtest_PCM2AACTools */

#ifndef _Included_com_thinking_pcmtest_PCM2AACTools
#define _Included_com_thinking_pcmtest_PCM2AACTools
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     com_thinking_pcmtest_PCM2AACTools
	 * Method:    test
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_thinking_pcmtest_PCM2AACTools_test
		(JNIEnv *, jclass);

	/*
	 * Class:     com_thinking_pcmtest_PCM2AACTools
	 * Method:    PcmFileToAccFile
	 * Signature: (Ljava/lang/String;)Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFile
		(JNIEnv *, jclass, jstring);

	/*
	 * Class:     com_thinking_pcmtest_PCM2AACTools
	 * Method:    PcmFileToAccFileOut
	 * Signature: (Ljava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFileOut
		(JNIEnv *, jclass, jstring);

	/*
	 * Class:     com_thinking_pcmtest_PCM2AACTools
	 * Method:    PcmFileToAccFileOutStop
	 * Signature: ()I
	 */
	JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_PCM2AACTools_PcmFileToAccFileOutStop
		(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
