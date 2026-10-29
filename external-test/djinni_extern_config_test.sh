#!/usr/bin/env bash
set -euo pipefail

"$1" \
    --idl extern_consumer.djinni \
    --cpp-out "$TEST_TMPDIR/cpp" \
    --java-out "$TEST_TMPDIR/java"

grep -q 'external::ExternalType' "$TEST_TMPDIR/cpp/consumer.hpp"
grep -q 'external.ExternalType' "$TEST_TMPDIR/java/Consumer.java"
test ! -e "$TEST_TMPDIR/cpp/external_type.hpp"
test ! -e "$TEST_TMPDIR/java/ExternalType.java"
