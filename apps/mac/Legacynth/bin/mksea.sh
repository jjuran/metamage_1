#!/bin/sh -e

TITLENAME="Legacynth Demo"
SMALLNAME="legacynth-demo"

VERS="$1"

if [ -z "$VERS" ]; then
	echo >&2 "### No version specified; will build but not package"
fi

BUILD_FLAGS=-R

BLD=opt

bin/rbuild $BUILD_FLAGS -B46 LegacynthINIT
bin/rbuild $BUILD_FLAGS -B56 LegacynthServer
bin/rbuild $BUILD_FLAGS -BFP LegacynthServer
bin/rbuild $BUILD_FLAGS -B56 SoundCheck
bin/rbuild $BUILD_FLAGS -B56 SEAshell

if [ -z "$VERS" ]; then
	exit
fi

REPO=~/src/metamage_1

TMP=~/mac/fs/tmp
NEW=~/var/new

A4=~/var/build/68k-a4-blue-$BLD/bin
A5=~/var/build/68k-a5-blue-$BLD/bin
PPC=~/var/build/ppc-cfm-blue-$BLD/bin

INIT="Legacynth Extension"
APP="Legacynth Server"
README="Legacynth — Read Me"
SNDCK="Sound Check"
SEASH="SEA Shell"

SUBDIR_APP="LegacynthServer/$APP"
SUBDIR_INIT="LegacynthINIT/$INIT"
SUBDIR_SNDCK="SoundCheck/$SNDCK"
SUBDIR_SEASH="SEAshell/$SEASH"

DIRNAME="$TITLENAME $VERS"
LOWNAME="$SMALLNAME-$VERS"

INSTALLER="$DIRNAME Installer"
INSTALLER_TMP="$DIRNAME temp"
INSTALLER_BIN="$LOWNAME-sea.bin"

REZ="Rez -a Carbon.r"

RCLEANSE="$REPO/v/bin/rcleanse.vx -q"

SRC="$REPO"/apps/mac/Legacynth
SND="$SRC"/SoundCheck/Rez/test._snd

README_MD="$SRC/README.md"
VERS2_R="$SRC/common/vers-2.r"

mkdir -p "$TMP/$DIRNAME" "$NEW"

cd "$TMP"

rm -f "$DIRNAME.mbim" "$INSTALLER_BIN"

mdstrip.pl "$README_MD" | pretty.pl | utf82mac | lf2cr > "$DIRNAME/$README"

SetFile -t TEXT -c ttxt "$DIRNAME/$README"

cp        "$A4/$SUBDIR_INIT"                         "$DIRNAME/"
fatten -K "$A5/$SUBDIR_APP" -P "$PPC/$SUBDIR_APP" -o "$DIRNAME/$TITLENAME"
cp        "$A5/$SUBDIR_SNDCK"                        "$DIRNAME/"
cp        "$A5/$SUBDIR_SEASH"                        "$INSTALLER_TMP"

$REZ -o "$DIRNAME/$INIT"      "$VERS2_R"
$REZ -o "$DIRNAME/$TITLENAME" "$VERS2_R"
$REZ -o "$DIRNAME/$SNDCK"     "$VERS2_R"

if [ -f "$SND" ]; then
	echo "read '_SND' (128, purgeable) \"$SND\";" > sound.r
	
	$REZ -o "$DIRNAME/$SNDCK" sound.r
	
	rm sound.r
else
	echo >&2 "### File test._snd not found, skipping"
fi

$RCLEANSE "$DIRNAME/$INIT"
$RCLEANSE "$DIRNAME/$TITLENAME"
$RCLEANSE "$DIRNAME/$SNDCK"

if [ -n "$TIME" ]; then
	SetFile -d "$TIME" -m "$TIME" "$DIRNAME"/*
	SetFile -d "$TIME" -m "$TIME" "$DIRNAME"
fi

macbin encode "$DIRNAME"

echo "read 'BIN+' (256) \"$DIRNAME.mbim\";" > archive.r

$REZ -o "$INSTALLER_TMP" ./archive.r $SRC/installer/*.r

$RCLEANSE "$INSTALLER_TMP"

if [ -n "$TIME" ]; then
	SetFile -d "$TIME" -m "$TIME" "$INSTALLER_TMP"
fi

rm "$DIRNAME.mbim" archive.r

mv "$INSTALLER_TMP" "$INSTALLER"

macbin encode "$INSTALLER" "$INSTALLER_BIN"

mv "$INSTALLER_BIN" "$NEW"

echo "$NEW/$INSTALLER_BIN"
