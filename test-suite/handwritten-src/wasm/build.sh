#!/bin/sh
LIB_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/support-lib
LIB_SRC=$LIB_DIR/wasm/*.cpp
GEN_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/test-suite/generated-src
GEN_SRC=$GEN_DIR/wasm/*.cpp
IMP_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/test-suite/handwritten-src/cpp
IMP_SRC=$IMP_DIR/*.cpp
emcc -std=c++17 -O0 --bind $IMP_SRC $LIB_SRC $GEN_SRC -I$GEN_DIR/cpp -I$LIB_DIR -I$LIB_DIR/wasm -I$IMP_DIR -o test_cpp.js -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=tests -s WASM_BIGINT=1
# -s NO_DISABLE_EXCEPTION_CATCHING=1
