CCACHE_EXISTS := $(shell ccache -V)
ifdef CCACHE_EXISTS
    CXX := ccache g++-8
endif

DEBUG ?= 1
INCLUDE = 
CXXFLAGS_BASE = -c -g -pthread -fPIC -std=c++17 -Wall -Wno-unknown-pragmas $(INCLUDE) -I$(OUT_DIR)

BIN_DIR_BASE:=../../bin/
OUT_DIR_BASE := .obj/
LIBS_BASE =  -lMy -lJdeSsl -ldl -lprotobuf -lssl -lcrypto -lboost_system -lmatio -lstdc++fs 
ifeq ($(DEBUG), 1)
	OUT_DIR=$(OUT_DIR_BASE)debug
	CXXFLAGS= $(CXXFLAGS_BASE) -O0 -fsanitize=address -fno-omit-frame-pointer  
	BIN_DIR=$(BIN_DIR_BASE)debug
	LIBS = -lasan $(LIBS_BASE)
else
	OUT_DIR=$(OUT_DIR_BASE)release
	CXXFLAGS= $(CXXFLAGS_BASE) -DNDEBUG -O3
	BIN_DIR=$(BIN_DIR_BASE)release
	LIBS = $(LIBS_BASE)
endif

OUTPUT = $(BIN_DIR)/DriveBackup

LIBFLAGS = -pthread -L$(BIN_DIR) -Wl,-z,origin -Wl,-rpath='$$ORIGIN' 

all: $(OUTPUT)
	$(NOECHO) $(NOOP)

OBJECTS = $(OUT_DIR)/main.o $(OUT_DIR)/Scan.o $(OUT_DIR)/Drive.o $(OUT_DIR)/Merge.o

$(OUTPUT): $(OUT_DIR)/stdafx.h.gch $(OBJECTS)
	$(CXX) $(LIBFLAGS) $(OBJECTS) -o$(OUTPUT)  $(LIBS)

$(OUT_DIR)/stdafx.h.gch: pc.h
	$(CXX) $(CXXFLAGS) ./pc.h -o $(OUT_DIR)/stdafx.h.gch -I/home/duffyj/code/libraries/spdlog/include -I../../Framework/source -I/home/duffyj/code/libraries/json/include

$(OUT_DIR)/main.o: main.cpp pc.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o$@

$(OUT_DIR)/%.o: ./%.cpp %.h pc.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) ./$< -o$@

clean:
	rm -rf -d $(OUT_DIR)/*.*
	rm -f $(OUTPUT)
