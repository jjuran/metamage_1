#!/bin/sh -e

TITLENAME="Legacynth Demo"
SMALLNAME="legacynth-demo"

VERS="$1"

if [ -z "$VERS" ]; then
	echo >&2 "### No version specified; will build but not package"
fi

bin/rbuild -B46 LegacynthINIT
bin/rbuild -B56 LegacynthServer
bin/rbuild -BFP LegacynthServer

if [ -z "$VERS" ]; then
	exit
fi

DIRNAME="$TITLENAME $VERS"
LOWNAME="$SMALLNAME-$VERS"

INIT="Legacynth Extension"
APP="Legacynth Server"
README="Legacynth README"

A4=~/var/build/68k-a4-blue-dbg/bin
A5=~/var/build/68k-a5-blue-dbg/bin
PPC=~/var/build/ppc-cfm-blue-dbg/bin

TMP=~/mac/fs/tmp

mkdir -p "$TMP/$DIRNAME"

cd "$TMP"

lf2cr < ~/src/metamage_1/mac/hacks/Legacynth/README.md > "$DIRNAME/$README"

SetFile -t TEXT -c ttxt "$DIRNAME/$README"

cp "$A4/LegacynthINIT/$INIT"  "$DIRNAME/"
cp "$A5/LegacynthServer/$APP"  "$DIRNAME/$APP 68K"
cp "$PPC/LegacynthServer/$APP" "$DIRNAME/$APP PPC"

macbin encode "$DIRNAME"

mv "$DIRNAME.mbim" ~/var/pub/hacks/"$LOWNAME.mbim"
