#!/usr/bin/env python3
"""Check that game assets and code agree.

Usage: check_assets.py SOURCE.cpp...

Follows #include "..." from the given sources, collects every string literal
that names an asset, and compares those with the assets git tracks. Fails on
an asset the code names but git lacks, and on a tracked asset nothing names.

macOS file lookups ignore case and Unicode normalization, so a path can work
here and break on Linux. Those paths are matched loosely and reported as
warnings rather than failures.
"""
import os
import re
import subprocess
import sys
import unicodedata

ASSET_EXT = (".png", ".jpg", ".jpeg", ".ttf", ".otf", ".mp3", ".wav", ".ogg")
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
LITERAL = re.compile(r'"([^"\n]+)"')


def loose(path):
    return unicodedata.normalize("NFC", path).casefold()


def reachable_sources(roots):
    seen, todo = set(), [os.path.normpath(r) for r in roots]
    while todo:
        path = todo.pop()
        if path in seen or not os.path.isfile(path):
            continue
        seen.add(path)
        text = open(path, encoding="utf-8", errors="replace").read()
        for inc in INCLUDE.findall(text):
            for base in (os.path.dirname(path), "."):
                candidate = os.path.normpath(os.path.join(base, inc))
                if os.path.isfile(candidate):
                    todo.append(candidate)
                    break
    return seen


def main(roots):
    sources = reachable_sources(roots)
    referenced = {}
    for src in sorted(sources):
        text = open(src, encoding="utf-8", errors="replace").read()
        for m in LITERAL.finditer(text):
            # ": " marks a log message that quotes a path, not a path.
            if m.group(1).lower().endswith(ASSET_EXT) and ": " not in m.group(1):
                line = text.count("\n", 0, m.start()) + 1
                referenced.setdefault(m.group(1), []).append(f"{src}:{line}")

    tracked = subprocess.run(
        ["git", "-c", "core.quotepath=off", "ls-files", "-z"],
        capture_output=True, text=True, check=True,
    ).stdout.split("\0")
    assets = [f for f in tracked
              if f.lower().endswith(ASSET_EXT) and not f.startswith("tests/")]
    exact = set(assets)
    by_loose = {}
    for f in assets:
        by_loose.setdefault(loose(f), []).append(f)

    problems = 0
    used = set()
    for ref, where in sorted(referenced.items()):
        if ref in exact:
            used.add(ref)
        elif loose(ref) in by_loose:
            used.update(by_loose[loose(ref)])
            print(f"warning: {ref!r} only matches {by_loose[loose(ref)]} "
                  f"ignoring case/normalization ({where[0]})")
        else:
            print(f"missing: {ref!r} has no tracked file ({', '.join(where)})")
            problems += 1
    for f in sorted(set(assets) - used):
        print(f"unreferenced: {f!r}")
        problems += 1

    print(f"{len(sources)} sources, {len(referenced)} asset paths, "
          f"{len(assets)} tracked assets, {problems} problems")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
