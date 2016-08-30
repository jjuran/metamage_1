LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := recall
LOCAL_SRC_FILES     := src/recall/stack_crawl.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := gear
LOCAL_SRC_FILES     := src/gear/hexadecimal.cc       \
                       src/gear/inscribe_decimal.cc  \
                       src/gear/parse_decimal.cc     \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := plus
LOCAL_SRC_FILES     := src/plus/datum_access.cc   \
                       src/plus/datum_alloc.cc    \
                       src/plus/extent.cc         \
                       src/plus/hexadecimal.cc    \
                       src/plus/own_string.cc     \
                       src/plus/string.cc         \
                       src/plus/string/concat.cc  \
                       src/plus/string_common.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := poseven
LOCAL_SRC_FILES     := src/poseven/functions/perror.cc  \
                       src/poseven/functions/socket.cc  \
                       src/poseven/functions/write.cc   \
                       src/poseven/types/errno_t.cc     \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := tcpcat
LOCAL_SRC_FILES     := tcpcat.cc src/Orion/Main.cc pump.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

LOCAL_STATIC_LIBRARIES := poseven plus gear recall

include $(BUILD_EXECUTABLE)
