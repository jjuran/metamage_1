LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := must
LOCAL_SRC_FILES  := src/must/write.c
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

LOCAL_MODULE        := sha256
LOCAL_SRC_FILES     := src/sha256/table.cc   \
                       src/sha256/rounds.cc  \
                       src/sha256/sha256.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := math
LOCAL_SRC_FILES     := src/math/integer.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := chars
LOCAL_SRC_FILES     := src/encoding/utf8.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := gear
LOCAL_SRC_FILES     := src/gear/compare.cc           \
                       src/gear/hexadecimal.cc       \
                       src/gear/inscribe_decimal.cc  \
                       src/gear/mince.cc             \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := plus
LOCAL_SRC_FILES     := src/plus/binary.cc             \
                       src/plus/datum_access.cc       \
                       src/plus/datum_alloc.cc        \
                       src/plus/decode_binoid_int.cc  \
                       src/plus/extent.cc             \
                       src/plus/decimal.cc            \
                       src/plus/hexadecimal.cc        \
                       src/plus/ibox.cc               \
                       src/plus/integer.cc            \
                       src/plus/integer_hex.cc        \
                       src/plus/nth_power_of_ten.cc   \
                       src/plus/own_string.cc         \
                       src/plus/posix/dirname.cc      \
                       src/plus/reverse.cc            \
                       src/plus/string.cc             \
                       src/plus/string/concat.cc      \
                       src/plus/string/mince.cc       \
                       src/plus/string_common.cc      \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := poseven
LOCAL_SRC_FILES     := src/poseven/extras/read_all.cc  \
                       src/poseven/extras/slurp.cc     \
                       src/poseven/functions/fstat.cc  \
                       src/poseven/functions/open.cc   \
                       src/poseven/types/errno_t.cc    \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := vlib
LOCAL_SRC_FILES     := src/vlib/array-utils.cc                 \
                       src/vlib/calc.cc                        \
                       src/vlib/collectible.cc                 \
                       src/vlib/error.cc                       \
                       src/vlib/eval.cc                        \
                       src/vlib/execute.cc                     \
                       src/vlib/functions.cc                   \
                       src/vlib/init.cc                        \
                       src/vlib/interpret.cc                   \
                       src/vlib/iterators/array_iterator.cc    \
                       src/vlib/iterators/full_iterator.cc     \
                       src/vlib/iterators/generic_iterator.cc  \
                       src/vlib/iterators/list_builder.cc      \
                       src/vlib/iterators/list_iterator.cc     \
                       src/vlib/iterators/range_iterator.cc    \
                       src/vlib/iterators/string_iterator.cc   \
                       src/vlib/list-utils.cc                  \
                       src/vlib/named_ops.cc                   \
                       src/vlib/new_line.cc                    \
                       src/vlib/ops.cc                         \
                       src/vlib/parse.cc                       \
                       src/vlib/peephole.cc                    \
                       src/vlib/precedence.cc                  \
                       src/vlib/quote.cc                       \
                       src/vlib/scope.cc                       \
                       src/vlib/string-utils.cc                \
                       src/vlib/stack.cc                       \
                       src/vlib/symbol.cc                      \
                       src/vlib/symbol_table.cc                \
                       src/vlib/symdesc.cc                     \
                       src/vlib/table-utils.cc                 \
                       src/vlib/throw.cc                       \
                       src/vlib/token.cc                       \
                       src/vlib/token_type.cc                  \
                       src/vlib/tracker.cc                     \
                       src/vlib/types.cc                       \
                       src/vlib/types/any.cc                   \
                       src/vlib/types/boolean.cc               \
                       src/vlib/types/byte.cc                  \
                       src/vlib/types/integer.cc               \
                       src/vlib/types/null.cc                  \
                       src/vlib/types/stdint.cc                \
                       src/vlib/types/string.cc                \
                       src/vlib/types/vector.cc                \
                       src/vlib/value.cc                       \
                       src/vlib/vbox.cc                        \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := vx
LOCAL_SRC_FILES     := vx/exception.cc vx/library.cc vx/posixfs.cc vx/main.cc
LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := vlib poseven plus gear chars math sha256 command more must

include $(BUILD_EXECUTABLE)
