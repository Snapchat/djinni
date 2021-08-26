#!/bin/sh
LIB_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/support-lib
LIB_SRC=$LIB_DIR/wasm/*.cpp
GEN_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/examples/generated-src
GEN_SRC=$GEN_DIR/wasm/*.cpp
IMP_DIR=/Users/li.feng/Snapchat/Dev/client/tools/sc-djinni/examples/handwritten-src/cpp
IMP_SRC=$IMP_DIR/*.cpp
emcc -std=c++17 -O0 --bind $IMP_SRC $LIB_SRC $GEN_SRC -I$GEN_DIR/cpp -I$LIB_DIR -I$LIB_DIR/wasm -o demo_cpp.js -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=client
