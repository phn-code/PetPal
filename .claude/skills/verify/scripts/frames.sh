#!/bin/sh
# Usage: frames.sh <scenario>
# Runs one pin scenario against the working tree and saves every frame it
# displays as a PNG. Writes build/verify/frames/<scenario>/index.tsv with one
# row per frame: index, window title, script step, PNG path. The goldens in
# tests/golden are never read or written.
set -eu
[ $# -eq 1 ] || { echo "usage: frames.sh <scenario>" >&2; exit 2; }
root=$(cd "$(dirname "$0")/../../../.." && pwd)
cd "$root" && make -s build/pin
out="$root/build/verify/frames/$1"
rm -rf "$out" && mkdir -p "$out/png" "$out/golden" "$out/run"
ln -s "$root/assets" "$out/run/assets"
(cd "$out/run" && PIN_UPDATE=1 PIN_GOLDEN="$out/golden" PIN_OUT="$out" PIN_DUMP="$out/png" \
  "$root/build/pin" "$1" 2>"$out/pin.log") || { cat "$out/pin.log"; exit 1; }
awk -F'\t' -v dir="$out/png" '{ print $1 "\t" $2 "\t" $3 "\t" dir "/" substr($4, 1, 16) ".png" }' \
  "$out/golden/$1.txt" >"$out/index.tsv"
echo "$(wc -l <"$out/index.tsv" | tr -d ' ') frames, $(ls "$out/png" | wc -l | tr -d ' ') distinct: $out/index.tsv"
