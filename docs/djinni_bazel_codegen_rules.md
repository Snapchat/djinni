# Djinni Bazel Code Generation Rules

Djinni supports two generation workflows: build-time Bazel actions and its existing
command-line generator. Both use the same compiler and options. The Bazel rules
write to `bazel-out`; the scripts still write to `generated-src` for projects using
other build systems. Existing checked-in-source targets remain available.

## External Repository Setup

The repository currently supports WORKSPACE-based Bazel 5.4.1, as pinned in
`.bazelversion`. Bzlmod support is not implemented. Add Djinni as an archive pinned
to a revision and SHA-256, or use a local checkout:

```python
local_repository(name = "djinni", path = "/path/to/djinni")

load("@djinni//bzl:deps.bzl", "djinni_deps")
djinni_deps()
load("@djinni//bzl:scala_config.bzl", "djinni_scala_config")
djinni_scala_config()
load("@djinni//bzl:setup_deps.bzl", "djinni_setup_deps")
djinni_setup_deps()
```

These helpers configure the generator's Scala/JVM dependencies. The consumer
does not need Djinni's example-only Android, Apple, Kotlin, or Emscripten workspace
setup. C++ compilation needs a configured C++ toolchain; Java compilation needs
a Java toolchain. JNI and WASM consumers must also supply their platform toolchains
and appropriate support-library dependencies.

## Base Declaration and Language Libraries

The API follows the `rules_proto` separation of source declarations, providers,
and language consumers. `djinni_library` is a Starlark rule. Language library
macros create a private codegen rule and an ordinary `cc_library`, `java_library`,
or `cc_binary`, returning the normal language provider to downstream consumers.
The shared action implementation is internal; each language macro selects only
its own output categories and runs a separate action.

```python
load("@djinni//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library")

djinni_library(
    name = "messages",
    idl = "messages.djinni",
    srcs = ["common.djinni"],
)

OUTPUTS = {
    "cpp_hdrs": ["generated/cpp/message.hpp", "generated/cpp/common.hpp"],
    "java_srcs": ["generated/java/Message.java", "generated/java/Common.java"],
}

cc_djinni_library(
    name = "messages_cc",
    deps = [":messages"],
    outs = OUTPUTS,
    cpp_namespace = "example",
    includes = ["generated/cpp"],
    visibility = ["//visibility:public"],
)

java_djinni_library(
    name = "messages_java",
    deps = [":messages"],
    outs = OUTPUTS,
    java_package = "example",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "app",
    srcs = ["main.cc"],
    deps = [":messages_cc"],
)
```

Output names must match the IDL and options. A large manifest can be loaded from
a checked-in `.bzl` file, as in `examples/djinni_outputs.bzl`. Output declarations
are package-relative paths, and must not overlap checked-in files or outputs from
another target. Use distinct output directories for separate option sets.

### `djinni_library`

- `idl`: required single root `.djinni` label.
- `srcs`: additional imported `.djinni`, YAML, or proto file labels.
- `deps`: other `djinni_library` targets contributing transitive import inputs.
- `idl_include_paths`: repository-relative search directories. For example,
  `schemas/vendor` refers to that directory in the repository defining this rule,
  including when loaded as `@djinni`. Absolute paths and `..` are rejected.
  Djinni also searches relative to each importing file.
- `verify`: defaults to `True`; parsing and type resolution run with
  `--skip-generation true`. Verification outputs are dependencies of language
  actions. Set to `False` to omit the separate verification action; generation
  still parses and resolves the IDL.
- `compiler`: executable label, defaulting to Djinni's `//src:djinni`, resolved
  relative to the rules repository and built in the execution configuration.

`DjinniInfo` carries the root IDL, a depset of transitive source files, a depset
of execution-root include paths, and a depset of verification outputs. Default
files contain this target's direct sources and verification outputs.

Imports must be declared through `srcs` or `deps`; include paths do not make files
action inputs. No host filesystem scanning occurs during analysis. `deps` models
import availability, rather than compiling imported declarations separately.

### Language Macros

| Macro | Selected output categories | Consumer |
| --- | --- | --- |
| `cc_djinni_library` | `cpp_srcs`, `cpp_hdrs` | `cc_library` |
| `java_djinni_library` | `java_srcs` | `java_library` |
| `jni_djinni_library` | `jni_srcs`, `jni_hdrs` | `cc_library` |
| `wasm_djinni_cc_binary` | `wasm_srcs`, `wasm_hdrs` | `cc_binary` |

Each accepts exactly one base target in `deps`, an `outs` dictionary, and Djinni
options as keyword arguments. Other known language categories in a shared
manifest are ignored. Unknown categories fail analysis. Boolean CLI options
accept Starlark booleans; omission preserves the compiler's default.

Use `cc_deps` or `java_deps` for compilation dependencies, including generated
libraries referenced by bridges. `srcs` adds handwritten sources; C++ and JNI
macros also accept `hdrs`. C++/JNI macros accept `includes`, `copts`, `linkopts`,
and `alwayslink`; WASM accepts `copts` and `linkopts`. All accept `visibility`.
`testonly`, `tags`, `features`, `deprecation`, and compatibility attributes are
forwarded to both the private codegen target and its consumer.

Pass additional native language attributes through `library_kwargs`, for example
`library_kwargs = {"defines": ["MY_FEATURE=1"], "linkstatic": True}` for C++ or
`library_kwargs = {"javacopts": ["-Xlint"]}` for Java. Do not repeat explicitly
named macro attributes in this dictionary. `compiler` selects the codegen
executable independently of the base target's verification compiler; when using
a custom compiler, set it consistently on both targets.

JNI needs the same C++ namespace/identifier options and Java package/field
identifier options as its C++ and Java declarations when these differ from
compiler defaults. WASM also needs matching C++ namespace/identifier options.
WASM bridge class/file names use `ident_jni_class` and `ident_jni_file`; set them
to match your WASM output manifest, even when no JNI output is generated.
Supply C++ and JNI support libraries
through `cc_deps`; for desktop JNI use `@djinni//support-lib:djinni-support-jni`.
The WASM macro produces bridge C++ sources; select an Emscripten toolchain through
your workspace's WASM build setup.

## Low-Level Codegen

`djinni_codegen` remains available for explicit generated-file integration,
including Objective-C, Objective-C++, TypeScript, and YAML. Dedicated compiled
library macros for those languages are not implemented yet.

```python
load("@djinni//bzl:djinni_codegen.bzl", "djinni_codegen")

djinni_codegen(
    name = "messages_ts",
    djinni = ":messages",
    outs = {"ts_srcs": ["generated/ts/messages.ts"]},
    ts_module = "messages",
)
```

Use either `djinni` (a `DjinniInfo` target) or the legacy `idl` with explicit
`srcs` and `idl_include_paths`. Supported categories are `cpp_srcs`, `cpp_hdrs`,
`java_srcs`, `jni_srcs`, `jni_hdrs`, `objc_srcs`, `objc_hdrs`, `objcpp_srcs`,
`objcpp_hdrs`, `wasm_srcs`, `wasm_hdrs`, `ts_srcs`, and `yaml_srcs`. They are also
output groups, along with `all`. There is no `generators` attribute: nonempty
output categories select generators. Each category must use one directory;
Objective-C, Objective-C++, and WASM source/header pairs each share a directory.

Actions use declared files, `ctx.actions.args`, and the compiler's
`FilesToRunProvider` so its runfiles are included. Shell generation scripts are
not invoked by the rules. Missing declared outputs fail the build; detection of
extra undeclared outputs and a general `djinni_manifest_test` remain future work.

## Command-Line Generation

For a project using another build system, the existing scripts remain supported:

```sh
./examples/run_djinni.sh
./perftest/run_djinni.sh
./test-suite/run_djinni.sh
```

These scripts use Bazel to build the compiler, then run the CLI and update their
`generated-src` directories. They do not depend on the new codegen rules.
For a custom IDL use `./src/run --idl ... --cpp-out ... --java-out ...`.
`./src/run-assume-built` runs a compiler that has already been built.

To run generation without Bazel installed on the consuming machine, build the
standalone JVM artifact once (or distribute it to that machine):

```sh
bazel build //src:djinni_deploy.jar
java -jar bazel-bin/src/djinni_deploy.jar \
    --idl /path/to/messages.djinni --cpp-out /path/to/generated/cpp
```

This path requires a compatible Java runtime, but no Bazel at generation time.
Building Djinni from source still requires Bazel. Generated files can be compiled
with CMake, Gradle, Xcode, or another build system.

## Verification

From the repository root, check analysis and build generated consumers:

```sh
bazel build --nobuild //:djinni-codegen-consumer-verification //test-suite:testsuite-main-djinni-codegen
bazel build //:djinni-codegen-consumer-verification
bazel test //bzl/tests:rule_tests
```

The separate `external-test` workspace exercises external rule loads, imported
IDL from another repository, separate language actions, compilation, and runtime
field access. Its CLI equivalence test compares ordinary CLI generation with
Bazel-generated C++/Java output:

```sh
cd external-test
bazel test //:consumer_tests
```

JNI and WASM targets in the root aggregate require their platform setup. The
generated JNI consumers use desktop JNI support; generated WASM consumers are
wrapped with `wasm_cc_binary` to select the Emscripten toolchain. The standalone
consumer tests use C++ and Java only.

On macOS with newer Xcode, Bazel 5's `wrapped_clang` may fail with
`missing LC_UUID load command`. Validation on Xcode 26.5 used the following
command-only workarounds for that wrapper and the bundled older zlib:

```sh
bazel test //:consumer_tests --repo_env=BAZEL_USE_CPP_ONLY_TOOLCHAIN=1 \
    --host_conlyopt=-std=c90 --host_conlyopt=-Dfdopen=fdopen
```

Run this from `external-test`. These flags are local compatibility workarounds,
not requirements of the codegen rules.
