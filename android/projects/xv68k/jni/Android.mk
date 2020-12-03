LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := must
LOCAL_SRC_FILES  := src/must/write.c src/must/pthread.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := more
LOCAL_SRC_FILES     := src/more/perror.cc
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
LOCAL_SRC_FILES     := src/gear/hexadecimal.cc       \
                       src/gear/inscribe_decimal.cc  \
                       src/gear/parse_decimal.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := logofwar
LOCAL_SRC_FILES     := src/logofwar/clock.cc   \
                       src/logofwar/levels.cc  \
                       src/logofwar/print.cc   \
                       src/logofwar/report.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := rasterlib
LOCAL_SRC_FILES     := src/raster/load.cc      \
                       src/raster/relay.cc     \
                       src/raster/screen.cc    \
                       src/raster/sync.cc      \
                       src/raster/validity.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := v68k
LOCAL_SRC_FILES     := v68k/conditional.cc        \
                       v68k/decode.cc             \
                       v68k/effective_address.cc  \
                       v68k/emulator.cc           \
                       v68k/fetch.cc              \
                       v68k/fetches.cc            \
                       v68k/instructions.cc       \
                       v68k/line_4.cc             \
                       v68k/load_store.cc         \
                       v68k/macros.cc             \
                       v68k/memory.cc             \
                       v68k/microcode.cc          \
                       v68k/ops.cc                \
                       v68k/state.cc              \
                       v68k/print.cc              \
                       v68k/update_CCR.cc         \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := v68k-libs
LOCAL_SRC_FILES     := auth/auth.cc                  \
                       callout/bridge.cc             \
                       callout/memory.cc             \
                       screen/lock.cc                \
                       screen/storage.cc             \
                       screen/surface.cc             \
                       screen/update.cc              \
                       syscall/bridge.cc             \
                       syscall/handler.cc            \
                       utils/load.cc                 \
                       utils/print_register_dump.cc  \
                       v68k-alloc/memory.cc          \
                       v68k-mac/dynamic_globals.cc   \
                       v68k-mac/memory.cc            \
                       v68k-mac/trap_dispatcher.cc   \
                       v68k-time/clock.cc            \
                       v68k-user/line_A_shim.cc      \
                       v68k-user/load.cc             \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := xv68k
LOCAL_SRC_FILES     := xv68k/diagnostics.cc    \
                       xv68k/memory.cc         \
                       xv68k/native.cc         \
                       xv68k/screen.cc         \
                       xv68k/shared_memory.cc  \
                       xv68k/sound.cc          \
                       xv68k/VIA.cc            \
                       xv68k/xv68k.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src $(LOCAL_PATH)/xv68k
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := v68k-libs v68k rasterlib logofwar gear command more must

include $(BUILD_EXECUTABLE)
