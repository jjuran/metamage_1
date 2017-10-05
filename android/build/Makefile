ANDROID_NDK ?= ${HOME}/opt/android/ndk
ANDROID_SDK ?= ${HOME}/opt/android/sdk

PLATFORMS := ${ANDROID_NDK}/platforms
NDK_BUILD := ${ANDROID_NDK}/ndk-build
ADB       := ${ANDROID_SDK}/platform-tools/adb

ANDROIDS := "${PLATFORMS}/android-"

API_LEVEL ?= `ls -d ${ANDROIDS}* | sed -e s/.*android-// | sort -n | head -n1`

NDK_PLATFORM ?= android-${API_LEVEL}

BIN_DIR     := libs/armeabi
INSTALL_DIR := /data/local/tmp/bin

SET_NDK_MSG := "Can't find ndk-build (ANDROID_NDK is unset)."
FIX_NDK_MSG := "Can't find ndk-build (ANDROID_NDK is set incorrectly)."

SET_SDK_MSG := "Can't find adb (ANDROID_SDK is unset)."
FIX_SDK_MSG := "Can't find adb (ANDROID_SDK is set incorrectly)."

default: build-ready
	${NDK_BUILD}

clean: build-ready
	${NDK_BUILD} clean

build-ready: jni project.properties
	@test -x ${NDK_BUILD} -o -n "${ANDROID_NDK}" || echo ${SET_NDK_MSG}
	@test -x ${NDK_BUILD} -o -n "${ANDROID_NDK}" || exit 1
	@test -x ${NDK_BUILD} || echo ${FIX_NDK_MSG}
	@test -x ${NDK_BUILD} || exit 1

install-ready:
	@test -x ${ADB} -o -n "${ANDROID_SDK}" || echo ${SET_SDK_MSG}
	@test -x ${ADB} -o -n "${ANDROID_SDK}" || exit 1
	@test -x ${ADB} || echo ${FIX_SDK_MSG}
	@test -x ${ADB} || exit 1

jni:
	@echo "Run make in a project directory, not in 'build'."
	@exit 1

project.properties:
	@echo "target=${NDK_PLATFORM}" > project.properties
	@cat project.properties

tidy:
	@find . -name .DS_Store | xargs rm -v

install: install-ready default
	@${ADB} shell "umask 022 && mkdir ${INSTALL_DIR}" | grep -v exists || true
	@chmod -w ${BIN_DIR}/*
	@find ${BIN_DIR} -type f -perm -111 -exec ${ADB} push {} ${INSTALL_DIR}/ ';'
