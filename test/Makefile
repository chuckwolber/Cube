VERSION := 0.0.2

CXXOPTI :=
CXXFLAGS := -g -Wall -Werror -Wextra -Wconversion -Wuninitialized -pedantic -std=c++11
CXX := g++

BUILD_DIR := build

CUBE = Algorithm.cpp Cube.cpp
CUBEOBJ = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CUBE))
ALLEXEC = test_cube test_algorithm

.PHONY: all clean $(ALLEXEC)

all: test

test: $(ALLEXEC)
	$(BUILD_DIR)/test_algorithm
	$(BUILD_DIR)/test_cube

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test_algorithm: $(BUILD_DIR)/test_algorithm
$(BUILD_DIR)/test_algorithm: test_algorithm.cpp $(CUBEOBJ) | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(CUBEOBJ) $< -o $@

test_cube: $(BUILD_DIR)/test_cube
$(BUILD_DIR)/test_cube: test_cube.cpp $(CUBEOBJ) | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(CUBEOBJ) $< -o $@

$(BUILD_DIR)/%.o: ../%.cpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
