LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := more
LOCAL_SRC_FILES     := src/more/mempcpy.c src/more/perror.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := gear
LOCAL_SRC_FILES     := src/gear/inscribe_decimal.cc
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
LOCAL_SRC_FILES     := src/poseven/functions/dup2.cc        \
                       src/poseven/functions/execvp.cc      \
                       src/poseven/functions/socketpair.cc  \
                       src/poseven/functions/waitpid.cc     \
                       src/poseven/types/errno_t.cc         \
                       src/poseven/types/fd_pair.cc         \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := unet
LOCAL_SRC_FILES     := src/unet/connect.cc         \
                       src/unet/connection.cc      \
                       src/unet/connection_box.cc  \
                       src/unet/pass_fd.cc         \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := freemount
LOCAL_SRC_FILES     := src/freemount/address.cc        \
                       src/freemount/event_loop.cc     \
                       src/freemount/frame.cc          \
                       src/freemount/queue_utils.cc    \
                       src/freemount/receiver.cc       \
                       src/freemount/requests.cc       \
                       src/freemount/send_lock.cc      \
                       src/freemount/send_queue.cc     \
                       src/freemount/write_in_full.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := fls
LOCAL_SRC_FILES     := fls.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := freemount unet poseven plus gear more

include $(BUILD_EXECUTABLE)
