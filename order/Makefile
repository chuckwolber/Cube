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

EXEC   := cli cli_aggregate server client
CUBE   := Algorithm Cube
CS     := Common Results UserResults ServerConfig UUID parson log

CLIENT := $(CUBE) $(CS)
SERVER := $(CUBE) $(CS) Allocator User Users Session

CUBEOBJS   := $(patsubst %,$(BUILD_DIR)/%.o,$(CUBE))
CLIENTOBJS := $(patsubst %,$(BUILD_DIR)/%.o,$(CLIENT))
SERVEROBJS := $(patsubst %,$(BUILD_DIR)/%.o,$(SERVER))

.PHONY: all builddir test_setup clean $(EXEC) $(CUBE) $(CLIENT) $(SERVER)

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

server: $(BUILD_DIR)/server
$(BUILD_DIR)/server: Server.cpp $(SERVEROBJS) | builddir test_setup
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) $(SERVEROBJS) $< $(LLIBS) -o $@

client: $(BUILD_DIR)/client
$(BUILD_DIR)/client: Client.cpp $(CLIENTOBJS) | builddir test_setup
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) $(CLIENTOBJS) $< $(LLIBS) -o $@

Allocator: $(BUILD_DIR)/Allocator.o
$(BUILD_DIR)/Allocator.o: Allocator.cpp Allocator.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

Common: $(BUILD_DIR)/Common.o
$(BUILD_DIR)/Common.o: Common.cpp Common.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

Results: $(BUILD_DIR)/Results.o
$(BUILD_DIR)/Results.o: Results.cpp Results.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

ServerConfig: $(BUILD_DIR)/ServerConfig.o
$(BUILD_DIR)/ServerConfig.o: ServerConfig.cpp ServerConfig.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

Session: $(BUILD_DIR)/Session.o
$(BUILD_DIR)/Session.o: Session.cpp Session.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

User: $(BUILD_DIR)/User.o
$(BUILD_DIR)/User.o: User.cpp User.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

Users: $(BUILD_DIR)/Users.o
$(BUILD_DIR)/Users.o: Users.cpp Users.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

UserResults: $(BUILD_DIR)/UserResults.o
$(BUILD_DIR)/UserResults.o: UserResults.cpp UserResults.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) $(SSLINC) -c $< -o $@

UUID: $(BUILD_DIR)/UUID.o
$(BUILD_DIR)/UUID.o: UUID.cpp UUID.hpp | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

Algorithm: $(BUILD_DIR)/Algorithm.o
$(BUILD_DIR)/Algorithm.o: ../Algorithm.cpp ../Algorithm.h | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

Cube: $(BUILD_DIR)/Cube.o
$(BUILD_DIR)/Cube.o: ../Cube.cpp ../Cube.h | builddir
	$(CXX) $(CXXFLAGS) $(CXXOPTI) -c $< -o $@

parson: $(BUILD_DIR)/parson.o
$(BUILD_DIR)/parson.o: parson.c parson.h | builddir
	$(CC) $(CCFLAGS) $(CCOPTI) -c $< -o $@

log: $(BUILD_DIR)/log.o
$(BUILD_DIR)/log.o: log.c log.h | builddir
	$(CC) $(CCFLAGS) $(CCOPTI) -c $< -o $@

test_setup: $(BUILD_DIR)/test_setup
$(BUILD_DIR)/test_setup: | builddir
	touch $@
	mkdir -p build/logs
	cp -a test/test_data/cube_order_data build/

clean:
	rm -rf $(BUILD_DIR)
