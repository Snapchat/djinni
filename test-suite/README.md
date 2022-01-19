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
Use `bazelisk test //test-suite:djinni-objc-tests //test-suite:djinni-java-tests`
to build and run Objective-C and Java tests.
