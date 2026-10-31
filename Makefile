SFML_PREFIX ?= $(shell brew --prefix sfml@2 2>/dev/null)

CXX ?= c++
CXXFLAGS ?= -std=c++17
SFML_CFLAGS = -I$(SFML_PREFIX)/include
SFML_LDFLAGS = -L$(SFML_PREFIX)/lib -Wl,-rpath,$(SFML_PREFIX)/lib \
               -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

BUILD = build

SOURCES = buttons.cpp src/minigames/math.cpp src/minigames/boxes.cpp \
          pet_stats.cpp AdultAvoWindow.cpp BabyAvoWindow.cpp \
          AdultGhostWindow.cpp BabyGhostWindow.cpp EatingAdultAvo.cpp \
          EatingBabyAvo.cpp EatingAdultGhostWindow.cpp EatingBabyGhost.cpp
HEADERS = $(wildcard *.h src/*.h src/*/*.h)

UNIT_SOURCES = UnitTestingMain.cpp pet_stats.cpp src/minigames/boxes.cpp

# The pin builds production sources with tests/pin/shim.h force-included.
# tests/pin/pin.cpp includes buttons.cpp itself, with main renamed.
PIN_SOURCES = $(filter-out buttons.cpp,$(SOURCES)) tests/pin/pin.cpp

.PHONY: all run test pin pin-update pin-images check-assets clean

all: $(BUILD)/Petpal

$(BUILD):
	mkdir -p $@

$(BUILD)/Petpal: $(SOURCES) $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) $(SOURCES) $(SFML_LDFLAGS) -o $@

$(BUILD)/UnitTesting: $(UNIT_SOURCES) $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) $(UNIT_SOURCES) $(SFML_LDFLAGS) -o $@

# Assets load by relative path, so binaries run from the repo root.
run: $(BUILD)/Petpal
	./$(BUILD)/Petpal

$(BUILD)/pin: $(SOURCES) $(HEADERS) tests/pin/shim.h tests/pin/pin.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -I. -include tests/pin/shim.h $(SFML_CFLAGS) $(PIN_SOURCES) $(SFML_LDFLAGS) -o $@

test: $(BUILD)/UnitTesting
	./$(BUILD)/UnitTesting

# Characterization pin: the hash of every frame the game draws, compared with
# tests/golden. A mismatch writes the actual frame to build/pin-out.
pin: $(BUILD)/pin
	tests/pin/run.sh

# Fails on an asset the code names but git lacks, or one nothing names.
check-assets:
	python3 tests/check_assets.py $(SOURCES) $(UNIT_SOURCES)

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
