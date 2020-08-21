# Snapchat Djinni

See the [original dropbox readme](README.dropbox.md) for the full Djinni docs.

## Working on Djinni in an IDE

You can load the project via Bazel

### Bazel

- Intellij with the Bazel and Scala plugin is required
- Configure the bazel binary, or download [Bazelisk](https://github.com/bazelbuild/bazelisk)
  and set it as the binary in the IDEA bazel settings.
- In Intellij, import a new Bazel project.
    - Workspace directory: `/Users/$HOME/Snapchat/Dev/client/tools/sc-djinni`
    - Import project view file: `WORKSPACE/bzl/ide/djinni.bazelproject`
- Similarly you can also use CLion if you wish to edit the C++ code
    - You can set up any of the cc_* targets after importing the workspace.

You can also use `bazel build //src:djinni` and `bazel run //src:djinni` to verify the build and binary from the command line.

## Future Work

- Add a proper djinni rule to bazel for generation
  - Main blocker here is djinni does not have deterministic outputs and bazel doesn't support this

## Modifications:

 - Replaced sbt and gyp with Bazel
 - Added move assigment operator to GlobalRef in all djinni_support.hpp files
 - Made JniClassInitializer constructor public to allow arbitrary additional initialization in JNI_OnLoad()
 - Speed up string passing between Java and C++
 - Eliminate CppProxy finalizers
 - Option to generate ObjC protocols
 - Option to generate function prologue
 - Added `outcome<>` builtin type
