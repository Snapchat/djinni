#!/usr/bin/env bash
set -euo pipefail

test -s "$TEST_SRCDIR/$TEST_WORKSPACE/$1"
test -s "$TEST_SRCDIR/$TEST_WORKSPACE/$2"
