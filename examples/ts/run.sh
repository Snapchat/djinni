#! /usr/bin/env bash
set -eu
cd examples/ts
tsc
browserify demo.js -o bundle.js
cp -f ../wasm/{textsort-wasm.js,textsort-wasm.wasm} ./
sleep 1 && python3 -mwebbrowser http://localhost:8000/demo.html &
python3 -m http.server
