VERSION := 0.0.2

CXXOPTI :=
CXXFLAGS := -g -Wall -Werror -Wextra -Wconversion -Wpedantic -std=c++17 -pthread
CXX := g++

CCOPTI :=
CCFLAGS := -g -Wall -Werror -Wextra -Wconversion -Wpedantic -std=c99
CC := gcc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LLIBS := -luuid -lstdc++fs -lssl -lcrypto
endif
ifeq ($(UNAME_S),Darwin)
	SSLINC := -I/usr/local/opt/openssl/include
	LLIBS := -L/usr/local/opt/openssl/lib -lssl -lcrypto
endif

BUILD_DIR = build

EXEC   := cli cli_aggregate
CUBE   := Algorithm Cube
CUBEOBJS   := $(patsubst %,$(BUILD_DIR)/%.o,$(CUBE))

.PHONY: all builddir clean $(EXEC) $(CUBE)

all: $(EXEC)

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

cli: $(BUILD_DIR)/cli
$(BUILD_DIR)/cli: cli.cpp $(CUBEOBJS) | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(CUBEOBJS) $< -o $@

cli_aggregate: $(BUILD_DIR)/cli_aggregate
$(BUILD_DIR)/cli_aggregate: cli_aggregate.cpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $< -o $@

Algorithm: $(BUILD_DIR)/Algorithm.o
$(BUILD_DIR)/Algorithm.o: ../Algorithm.cpp ../Algorithm.h | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

Cube: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: ../Cube.cpp ../Cube.h | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
