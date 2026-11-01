SFML_PREFIX ?= $(shell brew --prefix sfml@2 2>/dev/null)

CXX ?= c++
CXXFLAGS ?= -std=c++17
# Every build fails on a warning. SFML's headers are system headers, so only
# this repo's code can fail it.
WARNINGS = -Wall -Wextra -Werror
SFML_CFLAGS = -isystem $(SFML_PREFIX)/include
SFML_LDFLAGS = -L$(SFML_PREFIX)/lib -Wl,-rpath,$(SFML_PREFIX)/lib \
               -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

BUILD = build

SOURCES = src/main.cpp src/catalog.cpp src/stats.cpp src/save.cpp src/anim.cpp src/quote.cpp \
          src/scenes/menu.cpp src/scenes/select.cpp src/scenes/home.cpp src/scenes/shop.cpp \
          src/scenes/fridge.cpp src/scenes/info.cpp \
          src/minigames/boxes.cpp src/minigames/math.cpp
HEADERS = $(wildcard src/*.h src/*/*.h)

# Unit tests and the pin both build with tests/pin/shim.h force-included, which
# swaps the window, clock, mouse and audio for scripted fakes.
# tests/pin/pin.cpp includes src/main.cpp itself, with main renamed.
UNIT_SOURCES = tests/unit/unit_tests.cpp src/stats.cpp src/catalog.cpp src/anim.cpp \
               src/quote.cpp src/save.cpp src/minigames/boxes.cpp
PIN_SOURCES = $(filter-out src/main.cpp,$(SOURCES)) tests/pin/pin.cpp

.PHONY: all run test pin pin-update pin-images check-assets lint hooks check clean

all: $(BUILD)/Petpal

$(BUILD):
	mkdir -p $@

$(BUILD)/Petpal: $(SOURCES) $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(WARNINGS) $(SFML_CFLAGS) $(SOURCES) $(SFML_LDFLAGS) -o $@

$(BUILD)/UnitTesting: $(UNIT_SOURCES) $(HEADERS) tests/pin/shim.h | $(BUILD)
	$(CXX) $(CXXFLAGS) $(WARNINGS) -include tests/pin/shim.h $(SFML_CFLAGS) $(UNIT_SOURCES) $(SFML_LDFLAGS) -o $@

# Assets load by relative path, so binaries run from the repo root.
run: $(BUILD)/Petpal
	./$(BUILD)/Petpal

$(BUILD)/pin: $(SOURCES) $(HEADERS) tests/pin/shim.h tests/pin/pin.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(WARNINGS) -include tests/pin/shim.h $(SFML_CFLAGS) $(PIN_SOURCES) $(SFML_LDFLAGS) -o $@

test: $(BUILD)/UnitTesting
	./$(BUILD)/UnitTesting

# Characterization pin: the hash of every frame the game draws, compared with
# tests/golden. A mismatch writes the actual frame to build/pin-out.
pin: $(BUILD)/pin
	tests/pin/run.sh

# Fails on an asset the code names but git lacks, or one nothing names.
check-assets:
	python3 tests/check_assets.py $(SOURCES) $(UNIT_SOURCES)

# Fails on code that branches on which pet it is outside src/catalog.cpp, or
# that draws or maps clicks without src/scene.h. tests/lint.py lists the rules.
lint:
	python3 tests/lint.py

# Rejects a commit that changes a golden without naming it in the message.
hooks:
	git config core.hooksPath .githooks

# Everything a change must pass before it is committed.
check: hooks all test pin check-assets lint

# Rewrite the manifests. Only for intended behavior changes; review the diff of
# tests/golden and look at changed frames with pin-images.
pin-update: $(BUILD)/pin
	PIN_UPDATE=1 tests/pin/run.sh

# Render the frames a commit's goldens stand for: make pin-images REF=<commit>
pin-images:
	@test -n "$(REF)" || { echo "usage: make pin-images REF=<commit>"; exit 2; }
	tests/pin/images.sh $(REF)

clean:
	-for w in $(BUILD)/ref/*; do git worktree remove --force $$w 2>/dev/null; done
	rm -rf $(BUILD)
