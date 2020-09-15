LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := which
LOCAL_SRC_FILES     := src/must/write.c which.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_EXECUTABLE)
