LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := SobelFilter
LOCAL_SRC_FILES := com_example_testsobel_nativeSobelFilter.cpp aopencl.c
LOCAL_LDLIBS+=-L$(SYSROOT)/usr/lib -llog -ldl

include $(BUILD_SHARED_LIBRARY) 
 
