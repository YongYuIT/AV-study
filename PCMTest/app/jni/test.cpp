#include"com_thinking_pcmtest_PCM2AACTools.h"
#include <faac.h>
#include<android\log.h>

JNIEXPORT void JNICALL Java_com_thinking_pcmtest_PCM2AACTools_test
(JNIEnv *, jclass){
	faacEncHandle hEncoder;
	faacEncConfigurationPtr pConfiguration;
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "success");
}