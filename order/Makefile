VERSION=0.0.1

CC = g++
CXXFLAGS = -Wall -Werror -pedantic -std=c++11 -pthread

BUILD_DIR = build
CLASSES = cubeorder.cpp Cube.cpp Algorithm.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLASSES))

.PHONY: all clean cubeorder cubeorder-debug cubeorder_aggregate cubeorder_aggregate-debug $(patsubst %.cpp,%.o,$(CLASSES))

all: builddir cubeorder cubeorder-debug cubeorder_aggregate cubeorder_aggregate-debug

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

cubeorder_aggregate: $(BUILD_DIR)/cubeorder_aggregate
$(BUILD_DIR)/cubeorder_aggregate:
	$(CC) $(CXXFLAGS) cubeorder_aggregate.cpp -o $(BUILD_DIR)/cubeorder_aggregate

cubeorder_aggregate-debug: $(BUILD_DIR)/cubeorder_aggregate-debug
$(BUILD_DIR)/cubeorder_aggregate-debug:
	$(CC) $(CXXFLAGS) cubeorder_aggregate.cpp -g -o $(BUILD_DIR)/cubeorder_aggregate-debug

cubeorder: $(BUILD_DIR)/cubeorder
$(BUILD_DIR)/cubeorder: $(OBJS)
	$(CC) $(CXXFLAGS) build/cubeorder.o build/Algorithm.o build/Cube.o -o $@

cubeorder-debug: $(BUILD_DIR)/cubeorder-debug
$(BUILD_DIR)/cubeorder-debug: $(OBJS)
	$(CC) $(CXXFLAGS) build/cubeorder.o build/Algorithm.o build/Cube.o -g -o $@

cubeorder.o: $(BUILD_DIR)/cubeorder.o
$(BUILD_DIR)/cubeorder.o: cubeorder.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

Cube.o: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: ../Cube.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

Algorithm.o: $(BUILD_DIR)/Algorithm.o
$(BUILD_DIR)/Algorithm.o: ../Algorithm.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

