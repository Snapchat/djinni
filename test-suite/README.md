Djinni Test Suite
-----------------
The test suite provides a toolkit to test the behavior of Djinni generated code. It can be
run under Java (standalone JUnit runner) and macOS (via XCode xctest).

Djinni Files
----------
The djinni files are located in `djinni/`. all.djinni is a list of import for all files you would
like to be generated. After input files are changed, run `./run_djinni.sh` to regenerate files.

Testing
-------
`bazel test //test-suite:all` builds and then runs all tests.

Or `bazel test //test-suite:djinni-objc-tests` and `bazel test
//test-suite:djinni-java-tests` to run Objective-C and Java tests separately.
