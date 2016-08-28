LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := more
LOCAL_SRC_FILES     := src/more/mempcpy.c src/more/perror.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := sha256
LOCAL_SRC_FILES     := src/sha256/table.cc   \
                       src/sha256/rounds.cc  \
                       src/sha256/sha256.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := sha256sum
LOCAL_SRC_FILES     := src/gear/hexadecimal.cc sha256sum.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := sha256 more

include $(BUILD_EXECUTABLE)
