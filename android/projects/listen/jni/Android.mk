LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := listen
LOCAL_SRC_FILES     := src/gear/parse_decimal.cc listen.cc
LOCAL_SRC_FILES     += src/posix/listen_unix.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_EXECUTABLE)
