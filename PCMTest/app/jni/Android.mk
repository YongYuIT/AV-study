LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := faac
LOCAL_SRC_FILES := E:\AV-study\20171012001\AV-study\PCMTest\faac-sdk\arm\lib\libfaac.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := com_thinking_faac
LOCAL_SRC_FILES := test.cpp
LOCAL_C_INCLUDES += E:\AV-study\20171012001\AV-study\PCMTest\faac-sdk\arm\include
LOCAL_LDLIBS +=  -llog -ldl -lz
LOCAL_SHARED_LIBRARIES := faac
include $(BUILD_SHARED_LIBRARY)