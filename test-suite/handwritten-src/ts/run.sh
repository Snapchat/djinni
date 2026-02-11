#! /usr/bin/env bash
set -eu
cd test-suite/handwritten-src/ts
tsc
browserify main.js -o bundle.js
mkdir -p serve
cp -f test.html bundle.js ../../wasm/test-wasm.js ../../wasm/test-wasm.wasm serve/
sleep 1 && python3 -mwebbrowser http://localhost:8000/test.html &
cd serve && python3 -m http.server
