LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := must
LOCAL_SRC_FILES  := src/must/chdir.c src/must/write.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := command
LOCAL_SRC_FILES     := src/command/errors.cc src/command/get_option.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := plus
LOCAL_SRC_FILES     := src/plus/datum_access.cc   \
                       src/plus/datum_alloc.cc    \
                       src/plus/extent.cc         \
                       src/plus/own_string.cc     \
                       src/plus/string.cc         \
                       src/plus/string_common.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := poseven
LOCAL_SRC_FILES     := src/poseven/functions/close.cc \
                       src/poseven/functions/ioctl.cc \
                       src/poseven/functions/open.cc \
                       src/poseven/functions/perror.cc \
                       src/poseven/types/errno_t.cc \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := daemonize
LOCAL_SRC_FILES     := daemonize.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := poseven plus command must

include $(BUILD_EXECUTABLE)
