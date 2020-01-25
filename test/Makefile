VERSION=0.0.1

CC = g++
CXXFLAGS = -Wall -Werror -pedantic -std=c++11

BUILD_DIR = build
CLASSES_CUBE = test_cube.cpp Algorithm.cpp Cube.cpp
CLASSES_ALGO = test_algorithm.cpp Algorithm.cpp Cube.cpp
OBJS_CUBE = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLASSES_CUBE))
OBJS_ALGO = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLASSES_ALGO))

.PHONY: all clean \
        test_cube test_cube-debug $(patsubst %.cpp,%.o,$(CLASSES_CUBE)) \
        test_algorithm test_algorithm-debug $(patsubst %.cpp,%.o,$(CLASSES_ALGO))

all: builddir test_cube test_algorithm

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test_algorithm: $(BUILD_DIR)/test_algorithm
$(BUILD_DIR)/test_algorithm: $(OBJS_ALGO)
	$(CC) $(CXXFLAGS) $(OBJS_ALGO) -o $@
	$(BUILD_DIR)/test_algorithm

test_cube: $(BUILD_DIR)/test_cube
$(BUILD_DIR)/test_cube: $(OBJS_CUBE)
	$(CC) $(CXXFLAGS) $(OBJS_CUBE) -o $@
	$(BUILD_DIR)/test_cube

test_algorithm-debug: $(BUILD_DIR)/test_algorithm-debug
$(BUILD_DIR)/test_algorithm-debug: $(OBJS_ALGO)
	$(CC) $(CXXFLAGS) $(OBJS_ALGO) -g -o $@

test_cube-debug: $(BUILD_DIR)/test_cube-debug
$(BUILD_DIR)/test_cube-debug: $(OBJS_CUBE)
	$(CC) $(CXXFLAGS) $(OBJS_CUBE) -g -o $@

test_algorithm.o: $(BUILD_DIR)/test_algorithm.o
$(BUILD_DIR)/test_algorithm.o: test_algorithm.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

test_cube.o: $(BUILD_DIR)/test_cube.o
$(BUILD_DIR)/test_cube.o: test_cube.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

Algorithm.o: $(BUILD_DIR)/Algorithm.o
$(BUILD_DIR)/Algorithm.o: ../Algorithm.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

Cube.o: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: ../Cube.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

