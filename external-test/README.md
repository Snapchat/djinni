External Test
-------------
This folder provides a standalone Bzlmod module to mimic consuming Djinni from an external repository via Bazel.
Run `bazel test //:consumer_tests` from this folder. The tests generate C++ and
Java at build time, compile consumers, and exercise record and enum field values.
The schema imports IDL from the Djinni repository through a transitive provider
and repository-relative include path. A separate test runs the ordinary CLI and
compares its C++/Java output with the Bazel-generated files.

Run `bash ci/test-external-consumer.sh` from the repository root for the isolation
regression: it rejects platform-specific modules in the dependency graph, clears
the Android SDK/NDK repository environment, and compiles common C++, desktop JNI,
and Java runtime support. The core C++ consumer also exercises `DataView`.

To run the compiler directly: `bazel run @djinni//src:djinni -- <command line flags>`.
See [the rule documentation](../docs/djinni_bazel_codegen_rules.md) for setup,
output declarations, and generation without Bazel on the consuming machine.
