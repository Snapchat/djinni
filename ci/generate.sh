#!/usr/bin/env bash

set -euo pipefail

echo "--- Check Generated files"
./examples/run_djinni.sh
./perftest/run_djinni.sh
./test-suite/run_djinni.sh

if test -z "$(git status --porcelain)"; then
  echo "--- Success! Generated files are clean."
else
  echo "--- Failed! Generated files are dirty after running generator."
  git status
  git --no-pager diff --submodule=diff
  exit 1
fi
