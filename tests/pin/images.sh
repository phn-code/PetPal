#!/bin/sh
# Usage: images.sh <commit>. Checks the commit out in a worktree under build/,
# runs that commit's own pin there, and saves every frame it displays as
# build/pin-images/<short sha>/<hash prefix>.png. The goldens are hashes, so
# this is how to look at an expected frame.
set -eu
[ $# -eq 1 ] || { echo "usage: images.sh <commit>" >&2; exit 2; }
root=$(cd "$(dirname "$0")/../.." && pwd)
sha=$(git -C "$root" rev-parse --verify "$1^{commit}")
short=$(git -C "$root" rev-parse --short=10 "$sha")
tree="$root/build/ref/$short"
images="$root/build/pin-images/$short"

[ -d "$tree" ] || git -C "$root" worktree add --detach "$tree" "$sha"
mkdir -p "$images"
make -C "$tree" build/pin SFML_PREFIX="${SFML_PREFIX:-$(brew --prefix sfml@2 2>/dev/null)}"
PIN_DUMP="$images" "$tree/tests/pin/run.sh" || true
echo "expected frames of $short: $images ($(ls "$images" | wc -l | tr -d ' ') distinct images)"
