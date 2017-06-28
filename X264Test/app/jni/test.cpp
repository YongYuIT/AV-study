#include"com_hsdi_x264test_X264Test.h"
#include<android\log.h>

JNIEXPORT void JNICALL Java_com_hsdi_x264test_X264Test_CppTest
(JNIEnv *, jclass){
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "hello word");
	x264_param_t param;
	x264_param_default(&param);//»± °≥ı ºªØ
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "cpu %i", param.cpu);

}