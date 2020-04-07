LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := ed25519
LOCAL_SRC_FILES  := ed25519/ed25519.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/ed25519

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE     := must
LOCAL_SRC_FILES  := src/must/write.c src/must/pthread.c
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

LOCAL_MODULE        := md5
LOCAL_SRC_FILES     := src/md5/table.cc   \
                       src/md5/rounds.cc  \
                       src/md5/md5.cc
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
LOCAL_SRC_FILES     := src/charsets/MacRoman.cc        \
                       src/charsets/extended_ascii.cc  \
                       src/conv/mac_utf8.cc            \
                       src/encoding/utf8.cc            \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := gear
LOCAL_SRC_FILES     := src/gear/compare.cc           \
                       src/gear/find.cc              \
                       src/gear/hexadecimal.cc       \
                       src/gear/inscribe_decimal.cc  \
                       src/gear/mince.cc             \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := plus
LOCAL_SRC_FILES     := src/plus/binary.cc         \
                       src/plus/datum_access.cc   \
                       src/plus/datum_alloc.cc    \
                       src/plus/extent.cc         \
                       src/plus/hexadecimal.cc    \
                       src/plus/mac_utf8.cc       \
                       src/plus/own_string.cc     \
                       src/plus/posix/dirname.cc  \
                       src/plus/reverse.cc        \
                       src/plus/string.cc         \
                       src/plus/string/concat.cc  \
                       src/plus/string/mince.cc   \
                       src/plus/string_common.cc  \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := bignum
LOCAL_SRC_FILES     := src/bignum/decode_binoid_int.cc  \
                       src/bignum/decimal.cc            \
                       src/bignum/fraction.cc           \
                       src/bignum/gcd.cc                \
                       src/bignum/ibox.cc               \
                       src/bignum/integer.cc            \
                       src/bignum/integer_hex.cc        \
                       src/bignum/nth_power_of_ten.cc   \

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
                       src/poseven/types/cond.cc       \
                       src/poseven/types/mutex.cc      \
                       src/poseven/types/thread.cc     \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := vlib
LOCAL_SRC_FILES     := src/vlib/analyze.cc                     \
                       src/vlib/array-utils.cc                 \
                       src/vlib/assert.cc                      \
                       src/vlib/assign.cc                      \
                       src/vlib/calc.cc                        \
                       src/vlib/collectible.cc                 \
                       src/vlib/compare.cc                     \
                       src/vlib/dispatch/stringify.cc          \
                       src/vlib/equal.cc                       \
                       src/vlib/error.cc                       \
                       src/vlib/eval.cc                        \
                       src/vlib/execute.cc                     \
                       src/vlib/fold.cc                        \
                       src/vlib/function-utils.cc              \
                       src/vlib/functions.cc                   \
                       src/vlib/init.cc                        \
                       src/vlib/interpret.cc                   \
                       src/vlib/is_function.cc                 \
                       src/vlib/is_type.cc                     \
                       src/vlib/iterators/array_iterator.cc    \
                       src/vlib/iterators/byterange_iterator.cc\
                       src/vlib/iterators/full_iterator.cc     \
                       src/vlib/iterators/generic_iterator.cc  \
                       src/vlib/iterators/list_builder.cc      \
                       src/vlib/iterators/list_iterator.cc     \
                       src/vlib/iterators/range_iterator.cc    \
                       src/vlib/iterators/safe_list_builder.cc \
                       src/vlib/iterators/string_iterator.cc   \
                       src/vlib/lib/ed25519.cc                 \
                       src/vlib/list-utils.cc                  \
                       src/vlib/map-reduce.cc                  \
                       src/vlib/named_ops.cc                   \
                       src/vlib/namespaces/Math.cc             \
                       src/vlib/namespaces/V.cc                \
                       src/vlib/new_line.cc                    \
                       src/vlib/ops.cc                         \
                       src/vlib/parse.cc                       \
                       src/vlib/peephole.cc                    \
                       src/vlib/precedence.cc                  \
                       src/vlib/pure.cc                        \
                       src/vlib/quote.cc                       \
                       src/vlib/reduce.cc                      \
                       src/vlib/scope.cc                       \
                       src/vlib/startup.cc                     \
                       src/vlib/string-utils.cc                \
                       src/vlib/stack.cc                       \
                       src/vlib/symbol.cc                      \
                       src/vlib/symbol_table.cc                \
                       src/vlib/symdesc.cc                     \
                       src/vlib/table-utils.cc                 \
                       src/vlib/targets.cc                     \
                       src/vlib/throw.cc                       \
                       src/vlib/token.cc                       \
                       src/vlib/token_type.cc                  \
                       src/vlib/tracker.cc                     \
                       src/vlib/types.cc                       \
                       src/vlib/types/any.cc                   \
                       src/vlib/types/bareword.cc              \
                       src/vlib/types/boolean.cc               \
                       src/vlib/types/builtin.cc               \
                       src/vlib/types/byte.cc                  \
                       src/vlib/types/byterange.cc             \
                       src/vlib/types/endec/byte.cc            \
                       src/vlib/types/endec/stdint.cc          \
                       src/vlib/types/field.cc                 \
                       src/vlib/types/fraction.cc              \
                       src/vlib/types/integer.cc               \
                       src/vlib/types/integer/stdint.cc        \
                       src/vlib/types/invocation.cc            \
                       src/vlib/types/iterator.cc              \
                       src/vlib/types/lambda.cc                \
                       src/vlib/types/mb32.cc                  \
                       src/vlib/types/namespace.cc             \
                       src/vlib/types/null.cc                  \
                       src/vlib/types/packed.cc                \
                       src/vlib/types/pointer.cc               \
                       src/vlib/types/proc.cc                  \
                       src/vlib/types/range.cc                 \
                       src/vlib/types/record.cc                \
                       src/vlib/types/receiver.cc              \
                       src/vlib/types/stdint.cc                \
                       src/vlib/types/string.cc                \
                       src/vlib/types/table.cc                 \
                       src/vlib/types/term.cc                  \
                       src/vlib/types/type.cc                  \
                       src/vlib/types/unitary.cc               \
                       src/vlib/types/vbytes.cc                \
                       src/vlib/types/vector/metatype.cc       \
                       src/vlib/types/vector/type.cc           \
                       src/vlib/types/vector/vector.cc         \
                       src/vlib/value.cc                       \
                       src/vlib/vbox.cc                        \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src $(LOCAL_PATH)/ed25519
LOCAL_CPP_EXTENSION := .cc
LOCAL_CPP_FEATURES  += exceptions

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE        := vx
LOCAL_SRC_FILES     := vx/main.cc                  \
                       vx/channel/channel.cc       \
                       vx/channel/metatype.cc      \
                       vx/channel/state.cc         \
                       vx/empty_signal_handler.cc  \
                       vx/exception.cc             \
                       vx/file_descriptor.cc       \
                       vx/library.cc               \
                       vx/posixfs.cc               \
                       vx/sockets.cc               \
                       vx/thread.cc                \
                       vx/thread_state.cc          \

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/src $(LOCAL_PATH)/vx
LOCAL_CPP_EXTENSION := .cc

LOCAL_STATIC_LIBRARIES := vlib poseven bignum plus gear chars math md5 sha256 command more must ed25519

include $(BUILD_EXECUTABLE)
