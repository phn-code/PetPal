#!/bin/sh
# Runs every pin scenario in parallel, each in its own sandbox directory: the
# game loads assets by relative path and reads and writes pet_stats.txt in its
# working directory, so a sandbox of symlinks keeps scenarios (and your own
# save file) apart. PIN_UPDATE=1 rewrites the manifests instead of comparing.
# PIN_DUMP=<absolute dir> also saves every displayed frame there as a PNG.
set -u
root=$(cd "$(dirname "$0")/../.." && pwd)
bin="$root/build/pin"
golden="$root/tests/golden"
out="${PIN_OUT:-$root/build/pin-out}"
sandbox="$root/build/pin-sandbox"

rm -rf "$out" "$sandbox"
mkdir -p "$out" "$sandbox"

for scenario in $("$bin" --list); do
  dir="$sandbox/$scenario"
  mkdir "$dir"
  for f in "$root"/* "$root"/.[!.]*; do
    case "$(basename "$f")" in
      build | tests | .git | pet_stats.txt*) ;;
      *) [ -e "$f" ] && ln -s "$f" "$dir/" ;;
    esac
  done
  (
    cd "$dir" &&
      PIN_GOLDEN="$golden" PIN_OUT="$out" "$bin" "$scenario" >"$out/$scenario.log" 2>&1
    echo $? >"$out/$scenario.status"
  ) &
done
wait

failed=0
for scenario in $("$bin" --list); do
  grep '^PIN' "$out/$scenario.log"
  [ "$(cat "$out/$scenario.status")" = 0 ] || failed=1
done

if [ "${PIN_UPDATE:-0}" = 1 ]; then
  echo "manifests rewritten: $(cat "$golden"/*.txt | wc -l | tr -d ' ') lines in $golden"
fi

if [ "$failed" = 1 ]; then
  echo "pin FAILED: logs and actual frames are in $out"
  echo "To see an expected frame: make pin-images REF=<last good commit>,"
  echo "then open build/pin-images/<short sha>/<expected hash>.png (first 16 hex digits of the hash printed above)."
  exit 1
fi
echo "pin ok"
