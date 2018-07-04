LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := must
LOCAL_SRC_FILES  := src/must/pthread.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := more
LOCAL_SRC_FILES     := src/more/mempcpy.c src/more/perror.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := command
LOCAL_SRC_FILES     := src/command/errors.cc src/command/get_option.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := gear
LOCAL_SRC_FILES     := src/gear/compare.cc           \
                       src/gear/hexadecimal.cc       \
                       src/gear/inscribe_decimal.cc  \
                       src/gear/parse_decimal.cc     \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := plus
LOCAL_SRC_FILES     := src/plus/datum_access.cc       \
                       src/plus/datum_alloc.cc        \
                       src/plus/extent.cc             \
                       src/plus/hexadecimal.cc        \
                       src/plus/own_string.cc         \
                       src/plus/string.cc             \
                       src/plus/string/concat.cc      \
                       src/plus/string_common.cc      \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := poseven
LOCAL_SRC_FILES     := src/poseven/extras/read_all.cc \
                       src/poseven/extras/slurp.cc \
                       src/poseven/functions/fstat.cc \
                       src/poseven/functions/ftruncate.cc \
                       src/poseven/functions/open.cc \
                       src/poseven/functions/opendir.cc \
                       src/poseven/functions/pread.cc \
                       src/poseven/functions/pwrite.cc \
                       src/poseven/functions/read.cc \
                       src/poseven/functions/stat.cc \
                       src/poseven/functions/write.cc \
                       src/poseven/types/cond.cc \
                       src/poseven/types/errno_t.cc \
                       src/poseven/types/mutex.cc \
                       src/poseven/types/thread.cc \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := vfs
LOCAL_SRC_FILES     := src/vfs/dir_contents.cc \
                       src/vfs/dir_entry.cc \
                       src/vfs/node.cc \
                       src/vfs/node/types/anonymous.cc \
                       src/vfs/node/types/null.cc \
                       src/vfs/node/types/posix.cc \
                       src/vfs/filehandle.cc \
                       src/vfs/filehandle/functions/truncate.cc \
                       src/vfs/filehandle/primitives/append.cc \
                       src/vfs/filehandle/primitives/geteof.cc \
                       src/vfs/filehandle/primitives/get_file.cc \
                       src/vfs/filehandle/primitives/pread.cc \
                       src/vfs/filehandle/primitives/pwrite.cc \
                       src/vfs/filehandle/primitives/read.cc \
                       src/vfs/filehandle/primitives/seteof.cc \
                       src/vfs/filehandle/primitives/write.cc \
                       src/vfs/filehandle/types/posix.cc \
                       src/vfs/functions/access.cc \
                       src/vfs/functions/file-tests.cc \
                       src/vfs/functions/resolve_links_in_place.cc \
                       src/vfs/functions/resolve_pathname.cc \
                       src/vfs/primitives/geteof.cc \
                       src/vfs/primitives/hardlink.cc \
                       src/vfs/primitives/inode.cc \
                       src/vfs/primitives/listdir.cc \
                       src/vfs/primitives/lookup.cc \
                       src/vfs/primitives/parent.cc \
                       src/vfs/primitives/open.cc \
                       src/vfs/primitives/resolve.cc \
                       src/vfs/primitives/seteof.cc \
                       src/vfs/primitives/stat.cc \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := freemount
LOCAL_SRC_FILES     := src/freemount/data_flow.cc \
                       src/freemount/event_loop.cc \
                       src/freemount/frame.cc \
                       src/freemount/queue_utils.cc \
                       src/freemount/receiver.cc \
                       src/freemount/response.cc \
                       src/freemount/send_lock.cc \
                       src/freemount/send_queue.cc \
                       src/freemount/server.cc \
                       src/freemount/session.cc \
                       src/freemount/task.cc \
                       src/freemount/write_in_full.cc \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := freemountd
LOCAL_SRC_FILES     := freemountd.cc assert.cc
LOCAL_SRC_FILES     += src/posix/connect_path.cc src/posix/open_or_connect.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := freemount vfs poseven plus gear command more must

include $(BUILD_EXECUTABLE)
