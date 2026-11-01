#!/usr/bin/env python3
"""Fail on code in src/ that breaks a structural rule of this repo.

Usage: lint.py  (run from the repository root; make lint does this)

Each rule is a regex, the files allowed to match it, and the fix.
"""
import pathlib
import re
import sys

CATALOG = ("src/catalog.cpp", "src/catalog.h")
PET_FIX = ("branches on which pet this is. Put the difference in the pet's "
           "PetSpec entry (or a Species lookup) in src/catalog.cpp and read it here.")
RULES = [
    (re.compile(r"[!=]=\s*(Species|Stage)::|(Species|Stage)::\w+\s*[!=]=|case\s+(Species|Stage)::"),
     CATALOG, PET_FIX),
    (re.compile(r'"(adult|baby)_(avo|ghost)"'), CATALOG, PET_FIX),
    (re.compile(r"\bclear\s*\(\s*sf::Color|\b\w*[wW]indow\w*\s*(\.|->)\s*clear\s*\(|\bsetView\s*\("), ("src/scene.h",),
     "draws without the scene view. Start each frame with clearScene(window, colour) "
     "from src/scene.h, so a window of any size letterboxes the 1920x1080 scene."),
    (re.compile(r"\bmapPixelToCoords\s*\("), ("src/scene.h",),
     "maps a click without the scene view. Use toScene(window, x, y) from src/scene.h."),
]


def main():
    problems = 0
    for path in sorted(pathlib.Path("src").rglob("*")):
        if path.suffix not in (".cpp", ".h"):
            continue
        name = path.as_posix()
        for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            for pattern, allowed, fix in RULES:
                if name not in allowed and pattern.search(line):
                    print(f"{name}:{number}: {fix}\n    {line.strip()}")
                    problems += 1
    print(f"lint: {problems} problems")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
