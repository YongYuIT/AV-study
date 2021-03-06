LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := G:\WebRtc\FFmpeg-SDK\arm\lib\libavcodec-57.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := G:\WebRtc\FFmpeg-SDK\arm\lib\libavformat-57.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := G:\WebRtc\FFmpeg-SDK\arm\lib\libavutil-55.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := G:\WebRtc\FFmpeg-SDK\arm\lib\libswresample-2.so
include $(PREBUILT_SHARED_LIBRARY)

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

include $(CLEAR_VARS)
LOCAL_MODULE    := com_thinking_ffmpeg
LOCAL_SRC_FILES := ffmpeg_test.cpp
LOCAL_C_INCLUDES += G:\WebRtc\FFmpeg-SDK\arm\include
LOCAL_LDLIBS +=  -llog -ldl -lz
LOCAL_SHARED_LIBRARIES := avcodec avformat avutil
include $(BUILD_SHARED_LIBRARY)