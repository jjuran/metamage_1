NDK_HOME := ${HOME}/opt/android/ndk
NDK_BUILD := ${NDK_HOME}/ndk-build

INSTALL_DIR := /data/local/tmp/bin

default:
	${NDK_BUILD}

clean:
	${NDK_BUILD} clean

tidy:
	@find . -name .DS_Store | xargs rm -v

install: default
	adb shell "umask 022 && mkdir -p ${INSTALL_DIR}"
	@rm -f libs/armeabi/.DS_Store
	@chmod -w libs/armeabi/*
	adb push libs/armeabi/* ${INSTALL_DIR}/
