LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := faac
LOCAL_SRC_FILES := E:\AV-study\20171103001\AV-study\PCMTest\faac\libs\$(TARGET_ARCH_ABI)\libfaac.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := faad2
LOCAL_SRC_FILES := E:\AV-study\20171103001\AV-study\PCMTest\faad\libs\$(TARGET_ARCH_ABI)\libfaad2.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := com_thinking_faac
LOCAL_SRC_FILES := pcm_test.cpp \
    PcmFile2AacFile.cpp \
    pcm2aac_web.cpp
LOCAL_C_INCLUDES += E:\AV-study\20171103001\AV-study\PCMTest\faac\jni\include
LOCAL_LDLIBS +=  -llog -ldl -lz
LOCAL_SHARED_LIBRARIES := faac
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := com_thinking_faad
LOCAL_SRC_FILES := aac_test.cpp \
    AacFile2PcmFile.cpp \
    aac_web.cpp \
    tools.cpp
LOCAL_C_INCLUDES += E:\AV-study\20171103001\AV-study\PCMTest\faad\jni\include
LOCAL_LDLIBS +=  -llog -ldl -lz
LOCAL_SHARED_LIBRARIES := faad2
include $(BUILD_SHARED_LIBRARY)