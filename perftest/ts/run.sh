#! /usr/bin/env bash
cd perftest/ts
tsc
browserify perftest.js -o bundle.js
cp -f ../wasm/{perftest-wasm.js,perftest-wasm.wasm} ./
sleep 1 && python3 -mwebbrowser http://localhost:8000/perftest.html &
python3 -m http.server
