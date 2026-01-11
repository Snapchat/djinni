#!/usr/bin/env bash

set -euo pipefail

echo "--- Running tests"

# Create a temporary file for test output
TEST_LOG=$(mktemp)
trap "rm -f $TEST_LOG" EXIT

# Run bazel test, save output to file, and show it in real-time
set +e
bazel test --test_output=all //test-suite:djinni-java-tests //test-suite:djinni-objc-tests //test-suite:djinni-tests-c 2>&1 | tee "$TEST_LOG"
TEST_EXIT_CODE=$?
set -e

echo ""
echo "==================================================================="

# Check if tests passed
if [[ $TEST_EXIT_CODE -eq 0 ]]; then
  echo "✅ All tests passed"
  exit 0
fi

# Tests reported failure - check if it's the ObjC false failure
echo "Checking for ObjC test false failure pattern..."

# Look for the specific pattern in the log file
if grep -q "djinni-objc-tests.*FAILED" "$TEST_LOG" && \
   grep -q "TEST EXECUTE SUCCEEDED" "$TEST_LOG" && \
   grep -q "Executed.*tests, with 0 failures" "$TEST_LOG"; then
  
  echo ""
  echo "⚠️  ObjC tests reported as FAILED but all tests actually passed"
  echo "    This is a known issue with Bazel's Apple test runner"
  echo "    See: TEST_PREMATURE_EXIT_FILE false positive"
  echo ""
  
  # Check if other tests passed
  if grep -q "djinni-java-tests.*PASSED" "$TEST_LOG" && \
     grep -q "djinni-tests-c.*PASSED" "$TEST_LOG"; then
    echo "✅ All tests actually passed (Java: PASSED, C: PASSED, ObjC: PASSED with false failure)"
    echo "==================================================================="
    exit 0
  fi
fi

# If we get here, there's a real test failure
echo "❌ Tests failed"
echo "==================================================================="
exit $TEST_EXIT_CODE
