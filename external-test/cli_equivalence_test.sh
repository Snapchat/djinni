#!/usr/bin/env bash
set -euo pipefail

# Exercise the normal CLI, independent of the Starlark generation actions.
"$1" \
    --config schema.djinni.yaml \
    --verify-config \
    --idl schema.djinni \
    --idl-include-path "$(dirname "$2")" \
    --cpp-out "$TEST_TMPDIR/cpp" \
    --cpp-namespace consumer \
    --java-out "$TEST_TMPDIR/java" \
    --java-package consumer \
    --java-use-final-for-record false

# Run from the YAML alone and compare the generated sources with the
# command-line invocation above.
"$1" \
    --config schema.djinni.yaml \
    --idl schema.djinni \
    --idl-include-path "$(dirname "$2")" \
    --cpp-out "$TEST_TMPDIR/config-cpp" \
    --java-out "$TEST_TMPDIR/config-java"

for file in message.hpp shared.hpp status.hpp; do
    diff -u "generated/cpp/$file" "$TEST_TMPDIR/cpp/$file"
done
for file in Message.java Shared.java Status.java; do
    diff -u "generated/java/$file" "$TEST_TMPDIR/java/$file"
    diff -u "$TEST_TMPDIR/java/$file" "$TEST_TMPDIR/config-java/$file"
done
for file in message.hpp shared.hpp status.hpp; do
    diff -u "$TEST_TMPDIR/cpp/$file" "$TEST_TMPDIR/config-cpp/$file"
done
