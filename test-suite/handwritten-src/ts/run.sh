#! /usr/bin/env bash
cd test-suite/handwritten-src/ts
tsc
browserify main.js -o bundle.js
cp -f ../../wasm/{test-wasm.js,test-wasm.wasm} ./
sleep 1 && python3 -mwebbrowser http://localhost:8000/test.html &
python3 -m http.server
