External Test
-------------
This folder provides a standalone workspace to mimic consuming Djinni from an external repository via Bazel.
Run `bazel test //:consumer_tests` from this folder. The tests generate C++ and
Java at build time, compile consumers, and exercise record and enum field values.
The schema imports IDL from the Djinni repository through a transitive provider
and repository-relative include path. A separate test runs the ordinary CLI and
compares its C++/Java output with the Bazel-generated files.

To run the compiler directly: `bazel run @djinni//src:djinni -- <command line flags>`.
See [the rule documentation](../docs/djinni_bazel_codegen_rules.md) for setup,
output declarations, and generation without Bazel on the consuming machine.
