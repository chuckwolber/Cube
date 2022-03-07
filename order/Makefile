VERSION := 0.0.3

CXXOPTI :=
CXXFLAGS := -Wall -Werror -Wextra -Wconversion -Wpedantic -std=c++17 -pthread
CXX := g++

BUILD_DIR = build

EXEC   := cli
CUBE   := Algorithm Cube
CUBEOBJS   := $(patsubst %,$(BUILD_DIR)/%.o,$(CUBE))

.PHONY: all builddir clean $(EXEC) $(CUBE)

all: $(EXEC)

fast: CXXFLAGS += -Ofast
fast: clean $(EXEC)

debug: CXXFLAGS += -g
debug: clean $(EXEC)

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

cli: $(BUILD_DIR)/cli
$(BUILD_DIR)/cli: cli.cpp $(CUBEOBJS) | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(CUBEOBJS) $< -o $@

Algorithm: $(BUILD_DIR)/Algorithm.o
$(BUILD_DIR)/Algorithm.o: ../Algorithm.cpp ../Algorithm.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

Cube: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: ../Cube.cpp ../Cube.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
