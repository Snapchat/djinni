#!/usr/bin/env bash

set -euo pipefail

echo "--- Running tests"

# Run bazel test and capture output
TEST_OUTPUT=$(bazel test --test_output=all //test-suite:djinni-java-tests //test-suite:djinni-objc-tests //test-suite:djinni-tests-c 2>&1) || TEST_EXIT_CODE=$?

# Print the output
echo "$TEST_OUTPUT"

# Check if ObjC tests actually passed despite Bazel reporting failure
if [[ $TEST_EXIT_CODE -ne 0 ]]; then
  # Check if the ObjC test log shows all tests passed
  if echo "$TEST_OUTPUT" | grep -q "djinni-objc-tests.*FAILED" && \
     echo "$TEST_OUTPUT" | grep -q "TEST EXECUTE SUCCEEDED" && \
     echo "$TEST_OUTPUT" | grep -q "with 0 failures"; then
    echo ""
    echo "==================================================================="
    echo "⚠️  ObjC tests reported as FAILED but all tests actually passed"
    echo "    This is a known issue with Bazel's Apple test runner"
    echo "    See: TEST_PREMATURE_EXIT_FILE false positive"
    echo "==================================================================="
    echo ""
    
    # Check if other tests passed
    if echo "$TEST_OUTPUT" | grep -q "djinni-java-tests.*PASSED" && \
       echo "$TEST_OUTPUT" | grep -q "djinni-tests-c.*PASSED"; then
      echo "✅ All tests actually passed (Java: PASSED, C: PASSED, ObjC: PASSED with false failure)"
      exit 0
    fi
  fi
  
  # If we get here, there's a real test failure
  echo "❌ Tests failed"
  exit $TEST_EXIT_CODE
fi

echo "✅ All tests passed"
exit 0

