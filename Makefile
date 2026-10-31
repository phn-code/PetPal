SFML_PREFIX ?= $(shell brew --prefix sfml@2 2>/dev/null)

CXX ?= c++
CXXFLAGS ?= -std=c++11
SFML_CFLAGS = -I$(SFML_PREFIX)/include
SFML_LDFLAGS = -L$(SFML_PREFIX)/lib -Wl,-rpath,$(SFML_PREFIX)/lib \
               -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

BUILD = build

SOURCES = buttons.cpp MATHGAME.cpp Animation.cpp mini_game.cpp \
          pet_stats.cpp AdultAvoWindow.cpp BabyAvoWindow.cpp \
          AdultGhostWindow.cpp BabyGhostWindow.cpp EatingAdultAvo.cpp \
          EatingBabyAvo.cpp EatingAdultGhostWindow.cpp EatingBabyGhost.cpp
HEADERS = $(wildcard *.h)

UNIT_SOURCES = UnitTestingMain.cpp pet_stats.cpp mini_game.cpp

.PHONY: all run test clean

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

test: $(BUILD)/UnitTesting
	./$(BUILD)/UnitTesting

clean:
	rm -rf $(BUILD)
