#!/bin/sh -e

TMP=/tmp

REPO=~/src/metamage_1

SRC="$REPO"/apps/mac/SEAshell

DATA="$SRC"/data

PAYLOAD_TXT="$SRC"/sample/payload.txt
DEMO_README="$TMP/SEA Shell Demo — Read Me"
SAMPLE_MBIM="$DATA/sample.mbim"

mkdir -p "$DATA"

lf2cr < "$PAYLOAD_TXT" > "$DEMO_README"

SetFile -t TEXT -c ttxt "$DEMO_README"

touch -ct 202511160800 "$DEMO_README"

rm -f "$SAMPLE_MBIM"

macbin encode "$DEMO_README" "$SAMPLE_MBIM"

rm "$DEMO_README"
