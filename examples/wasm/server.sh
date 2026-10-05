#!/usr/bin/env bash
set -euo pipefail

module=$1
entry=$2
shift 2
stage=$(mktemp -d)
trap 'rm -rf "$stage"' EXIT

# Compile in a writable staging tree; Bazel runfiles are inputs, not outputs.
for group in "$@"; do
    read -r -a files <<< "$group"
    for source in "${files[@]}"; do
        case "$source" in
            */examples/*) relative="examples/${source#*/examples/}" ;;
            */perftest/*) relative="perftest/${source#*/perftest/}" ;;
            */support-lib/*) relative="support-lib/${source#*/support-lib/}" ;;
            *.js|*.wasm) relative="$module/ts/${source##*/}" ;;
            *) continue ;;
        esac
        # WASM output paths may include the original C++ target's package.
        case "$source" in
            *.js|*.wasm) relative="$module/ts/${source##*/}" ;;
        esac
        mkdir -p "$stage/$(dirname "$relative")"
        cp "$source" "$stage/$relative"
    done
done

cd "$stage/$module/ts"
tsc
browserify "$entry.js" -o bundle.js
echo "http://localhost:8000/$entry.html"
python3 -m http.server
