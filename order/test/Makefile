VERSION := 0.0.2

CXXOPTI :=
CXXFLAGS := -g -Wall -Werror -Wextra -Wconversion -Wuninitialized -pedantic -std=c++17
CXX := g++

CCOPTI :=
CCFLAGS := -g -Wall -Werror -Wextra -Wconversion -Wpedantic -std=c99
CC := gcc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	UUID := -luuid
endif

BUILD_DIR := build

LIBALLOC = Algorithm Allocator ServerConfig Session UUID parson log
OBJALLOC = $(patsubst %,$(BUILD_DIR)/%.o,$(LIBALLOC))
LIBRSLTS = ServerConfig UUID log parson
OBJRSLTS = $(patsubst %,$(BUILD_DIR)/%.o,$(LIBRSLTS))
ALLEXEC = test_allocator test_results

.PHONY: all clean test_setup test $(ALLEXEC)

all: $(ALLEXEC)

test: $(ALLEXEC)
	$(BUILD_DIR)/test_allocator
	$(BUILD_DIR)/test_results

builddir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test_allocator: $(BUILD_DIR)/test_allocator
$(BUILD_DIR)/test_allocator: test_allocator.cpp $(OBJALLOC) | builddir test_setup
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(UUID) $(OBJALLOC) $< -o $@

test_results: $(BUILD_DIR)/test_results
$(BUILD_DIR)/test_results: test_results.cpp $(OBJRSLTS) | builddir test_setup
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(UUID) $(OBJRSLTS) $< -o $@

$(BUILD_DIR)/Algorithm.o: ../../Algorithm.cpp ../../Algorithm.h | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@
$(BUILD_DIR)/Allocator.o: ../Allocator.cpp ../Allocator.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@
$(BUILD_DIR)/ServerConfig.o: ../ServerConfig.cpp ../ServerConfig.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@
$(BUILD_DIR)/Session.o: ../Session.cpp ../Session.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@
$(BUILD_DIR)/UUID.o: ../UUID.cpp ../UUID.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@
$(BUILD_DIR)/Results.o: ../Results.cpp ../Results.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

$(BUILD_DIR)/parson.o: ../parson.c ../parson.h | builddir
	$(CC) $(CCFLAGS) $(CCOPTI) -c $< -o $@
$(BUILD_DIR)/log.o: ../log.c ../log.h | builddir
	$(CC) $(CCFLAGS) $(CCOPTI) -c $< -o $@

test_setup: $(BUILD_DIR)/test_setup
$(BUILD_DIR)/test_setup: | builddir
	cp -a test_data/cube_order_data build/

clean:
	rm -rf $(BUILD_DIR)
