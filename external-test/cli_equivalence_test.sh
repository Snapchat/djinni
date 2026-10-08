#!/usr/bin/env bash
set -euo pipefail

# Exercise the normal CLI, independent of the Starlark generation actions.
"$1" \
    --idl schema.djinni \
    --idl-include-path "$(dirname "$2")" \
    --cpp-out "$TEST_TMPDIR/cpp" \
    --cpp-namespace consumer \
    --java-out "$TEST_TMPDIR/java" \
    --java-package consumer \
    --java-use-final-for-record false

for file in message.hpp shared.hpp status.hpp; do
    diff -u "generated/cpp/$file" "$TEST_TMPDIR/cpp/$file"
done
for file in Message.java Shared.java Status.java; do
    diff -u "generated/java/$file" "$TEST_TMPDIR/java/$file"
done
