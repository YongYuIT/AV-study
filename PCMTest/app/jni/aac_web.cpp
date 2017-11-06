#include"com_thinking_pcmtest_AAC2PCMTools.h"
#include<android\log.h>
#include<iostream>
#include<fstream>
using namespace std;

static const int FRAME_MAX_LEN = 1024 * 5;
static const int ADTS_HEAD = 10;

static ifstream* infile = NULL;
static long index = 0;
static unsigned char head[ADTS_HEAD] = { 0 };
static unsigned char frame[FRAME_MAX_LEN] = { 0 };

static long maxLength = 0;

JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_GetAACFrame
(JNIEnv * env, jclass j_class, jstring aac_file_path)
{

	if (infile == NULL){
		string aac_path = env->GetStringUTFChars(aac_file_path, 0);
		infile = new ifstream(aac_path.c_str(), ifstream::binary);

		infile->seekg(0, ios::end);
		maxLength = infile->tellg();

		infile->seekg(0, ios::beg);
		index = 0;
	}

	while (true)
	{
		if (infile->eof() || index >= maxLength){
			__android_log_print(ANDROID_LOG_INFO, "yuyong_send_web", "loop end========================================================");
			infile->clear();
			infile->seekg(0, ios::beg);
			index = 0;
		}
		memset(head, 0, ADTS_HEAD);
		infile->read((char*)head, ADTS_HEAD);
		__android_log_print(ANDROID_LOG_INFO, "yuyong_send_web", " try read %i of %i", index, maxLength);
		if ((head[0] == 0xff) && ((head[1] & 0xf0) == 0xf0)){
			size_t frame_size = 0;
			frame_size |= ((head[3] & 0x03) << 11);     //high 2 bit
			frame_size |= head[4] << 3;                 //middle 8 bit
			frame_size |= ((head[5] & 0xe0) >> 5);      //low 3bit
			__android_log_print(ANDROID_LOG_INFO, "yuyong adts", "adts-->%i", frame_size);
			infile->seekg(index, ios::beg);
			memset(frame, 0, FRAME_MAX_LEN);
			infile->read((char*)frame, frame_size);
			__android_log_print(ANDROID_LOG_INFO, "yuyong adts", "read success");
			//»Øµ÷
			jfieldID _mCache_id = env->GetStaticFieldID(j_class, "mCache", "[B");
			jobject _mCache = env->GetStaticObjectField(j_class, _mCache_id);
			unsigned char * c_mCache = (unsigned char *)env->GetByteArrayElements((jbyteArray)_mCache, 0);
			memcpy(c_mCache, frame, frame_size);
			env->ReleaseByteArrayElements((jbyteArray)_mCache, (jbyte*)c_mCache, JNI_COMMIT);
			index += frame_size;
			__android_log_print(ANDROID_LOG_INFO, "yuyong adts", "callback success");
			return frame_size;
		}
		else{
			index++;
		}
		infile->seekg(index, ios::beg);
	}
	return -1;
}
JNIEXPORT jint JNICALL Java_com_thinking_pcmtest_AAC2PCMTools_GetAACFrameStop
(JNIEnv *, jclass)
{
	infile->close();
	delete infile;
	infile = NULL;
}