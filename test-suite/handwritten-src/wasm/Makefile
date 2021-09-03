CC=emcc
CXX=emcc

ROOT=$(shell git rev-parse --show-toplevel)
LIB_DIR=$(ROOT)/support-lib
GEN_DIR=$(ROOT)/test-suite/generated-src
IMP_DIR=$(ROOT)/test-suite/handwritten-src/cpp

TARGET ?= test_cpp.js
SRC_DIRS ?= $(LIB_DIR)/wasm $(LIB_DIR)/cpp $(GEN_DIR)/wasm $(IMP_DIR) .

SRCS := $(shell find $(SRC_DIRS) -name *.cpp ! -name *Proto*)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(GEN_DIR)/cpp $(LIB_DIR) $(LIB_DIR)/wasm $(LIB_DIR)/cpp $(IMP_DIR)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -std=c++17 --bind -Os
LDFLAGS ?= --bind -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=tests -s WASM_BIGINT=1 -Os

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
