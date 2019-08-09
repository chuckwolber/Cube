VERSION=0.0.1

CC = g++
CXXFLAGS = -Wall -Werror -pedantic -std=c++11

BUILD_DIR = build
CLASSES = cubetest.cpp Cube.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLASSES))

.PHONY: all clean cubetest cubetest-debug $(patsubst %.cpp,%.o,$(CLASSES))

all: builddir cubetest

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

cubetest: $(BUILD_DIR)/cubetest
$(BUILD_DIR)/cubetest: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $@

cubetest-debug: $(BUILD_DIR)/cubetest-debug
$(BUILD_DIR)/cubetest-debug: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -g -o $@

cubetest.o: $(BUILD_DIR)/cubetest.o
$(BUILD_DIR)/cubetest.o: cubetest.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

Cube.o: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: Cube.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
