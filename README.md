# Snapchat Djinni

See the [original dropbox readme](README.dropbox.md) for the full Djinni docs.

## Working on Djinni in an IDE

You can either load the project via SBT or Bazel

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

`run_djinni.sh` still uses sbt for builds since bazel isn't pervasive at snap yet.

## Future Work

- Use bazel to create a deployment jar to use in client instead of sbt and clean up the remaining sbt stuff.
- Add a proper djinni rule to bazel for generation
  - Main blocker here is djinni does not have deterministic outputs and bazel doesn't support this


## Modifications:

 - Added move assigment operator to GlobalRef in all djinni_support.hpp files
 - Made JniClassInitializer constructor public to allow arbitrary additional initialization in JNI_OnLoad()
 - Speed up string passing between Java and C++
 - Eliminate CppProxy finalizers
 - Option to generate ObjC protocols
