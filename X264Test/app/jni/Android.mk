LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := G:\WebRtc\FFmpeg-SDK\arm\lib\libx264-150.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := com_thinking_x264
LOCAL_SRC_FILES := test.cpp \
                   x264_test.cpp \
                   Encoder.cpp


LOCAL_C_INCLUDES += G:\WebRtc\FFmpeg-SDK\arm\include
LOCAL_LDLIBS +=  -llog -ldl -lz
LOCAL_SHARED_LIBRARIES := x264
include $(BUILD_SHARED_LIBRARY)
