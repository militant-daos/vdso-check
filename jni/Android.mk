LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := maps.cpp
LOCAL_MODULE := maps
LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_CPP_INCLUDES += bionic
LOCAL_LDLIBS += -fPIE -pie
include $(BUILD_EXECUTABLE)
