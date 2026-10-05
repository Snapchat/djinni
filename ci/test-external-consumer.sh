#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root/external-test"

# Do not let an installed SDK/NDK mask a transitive platform dependency.
flags=(--repo_env=ANDROID_HOME= --repo_env=ANDROID_NDK_HOME=)
graph=$(bazel mod graph)
# rules_jvm_external includes Android rule definitions for AARs, not an NDK.
if grep -Eq '(rules_android_ndk|rules_swift|rules_apple|emsdk|snap_djinni_apple)@' <<< "$graph"; then
    printf '%s\n' "$graph" >&2
    echo "Core Djinni unexpectedly depends on a platform toolchain module." >&2
    exit 1
fi

bazel run "${flags[@]}" @djinni//src:djinni -- --help
bazel test "${flags[@]}" --test_output=errors //:consumer_tests
bazel build "${flags[@]}" //:runtime_support_verification
