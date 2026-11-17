# Djinni Bazel Code Generation Rules

Djinni supports two generation workflows: build-time Bazel actions and its existing
command-line generator. Both use the same compiler and options. The Bazel rules
write to `bazel-out`; the scripts still write to `generated-src` for projects using
other build systems. Existing checked-in-source targets remain available.

## External Repository Setup

The internal release branch uses Bazel 8.5.0, as pinned in `.bazelversion`,
and Bzlmod. For a local checkout, add the following to the consumer's
`MODULE.bazel`:

```python
bazel_dep(name = "snap_djinni", version = "1.0.0", repo_name = "djinni")
local_path_override(module_name = "snap_djinni", path = "/path/to/djinni")

bazel_dep(name = "rules_scala", version = "7.1.5")
scala_config = use_extension("@rules_scala//scala/extensions:config.bzl", "scala_config")
scala_config.settings(scala_version = "2.11.12")
```

Djinni's module configures its Scala/JVM dependencies. Scala configuration tags
are read from the root module, so consumers must select Scala 2.11.12 as above.
The local override can be replaced with an `archive_override` pinned to a
revision and integrity hash. C++ compilation needs a configured C++ toolchain; Java compilation needs
a Java toolchain. JNI and WASM consumers must also supply their platform toolchains
and appropriate support-library dependencies.

Codegen consumers do not need `ANDROID_NDK_HOME` just to compile or run the
generator. Core Djinni never configures or registers an Android SDK/NDK.
Consumers own the toolchains for generated code; existing parent toolchains
are selected by Bazel's normal platform/toolchain resolution.

## Platform Modules

Platform examples are separate Bzlmod roots, ignored by the core workspace's
recursive builds. Their `MODULE.bazel` files import core Djinni by local path
and configure only the platform they need:

| Root | Targets | Toolchain configuration |
| --- | --- | --- |
| `examples/android` | `//:android-app`, `//:perftest` | Android SDK/NDK; `ANDROID_HOME`, `ANDROID_NDK_HOME` |
| `examples/ios` | `//:textsort-swift-bridge` | Apple CC and Swift; requires Xcode |
| `examples/wasm` | `//:wasm-generated`, `//:perftest-wasm-generated` | Emscripten |

Run Bazel from the selected root, not the repository root. For example:

```sh
cd examples/android
bazel build //:android-app
```

The Swift example uses the optional Apple module's `swift_djinni_library`
macro to generate and compile Swift and Swift/C++ bridge libraries; the
existing Xcode application project remains available. WASM has
`//:codegen-consumer-verification` for both
generated consumers and `//:server` / `//:perftest-server` for the web demos.
These platform targets replace the previous root `//examples` application,
Swift, and WASM-wrapper targets and root `//perftest` application/WASM wrappers.
Core C++/Java/JNI generated-library targets retain their existing labels.

Swift runtime support is an optional companion module. A source-checkout
consumer can add this alongside its existing core dependency:

```python
bazel_dep(name = "snap_djinni_apple", version = "1.0.0", repo_name = "djinni_apple")
local_path_override(module_name = "snap_djinni_apple", path = "/path/to/djinni/support-lib/apple")
```

Use `@djinni_apple//:djinni-support-swift` and
`@djinni_apple//:djinni-support-swiftxx`. The companion module supplies Swift
Protobuf but does not register an Android NDK or override the parent's Apple
toolchains. The core repository's old Swift support labels remain aliases for
root-workspace tests only; external Swift consumers use the companion directly.
Neither module is assumed to be published in BCR; use checkout overrides or
pinned archives with the appropriate `strip_prefix` for each module root.

Load `swift_djinni_library` from
`@djinni_apple//:swift_djinni_codegen.bzl`. The macro uses the declaration lists
to select Swift and Swift/C++ bridge sources by owner; it splits public and
private Swift files into separate modules. Swift source access is file scoped,
so this compiled macro requires type metadata instead of using unity fallback.
Set `module_name` to the Swift module configured for the IDL, and provide the
generated native C++ library through `cc_deps` when the bridge uses native
types. See `examples/ios/BUILD` for a complete consumer.

Apple and Emscripten dependencies used by the legacy root test suite are
development-only. Run Apple tests with `--config=apple` and WASM test targets
with `--config=wasm`. For a core-only root build, use `--ignore_dev_dependency`.
The ordinary CLI and source-tree generation scripts are unchanged.

### Isolation Verification

`bash ci/test-external-consumer.sh` verifies a real external consumer with empty
Android SDK/NDK repository environment values, builds common/Java/desktop JNI
support, and runs C++/Java consumer and CLI-equivalence tests. It also rejects
NDK, Swift, Apple application-rule, and Emscripten modules in the dependency
graph. JVM tooling may include Android rule definitions for AAR support; that
does not configure an Android SDK or NDK. CI runs this check on macOS and Linux.

Consumers that use their own compiler can supply a `compiler` executable on
both the base and language targets. See `external-test/MODULE.bazel` for a
working external-module setup and explicit C++/Java rule dependencies.

## Base Declaration and Language Libraries

The API follows the `rules_proto` separation of source declarations, providers,
and language consumers. `djinni_library` is a Starlark rule. Language library
macros create a private codegen rule and an ordinary `cc_library`, `java_library`,
or `cc_binary`, returning the normal language provider to downstream consumers.
The shared action implementation is internal; each language macro selects only
its own output categories and runs a separate action.

`djinni_library` can carry the resolved enum, record, and interface names. Native language rules use those
types to expose one source artifact per possible generated type; a generator
ownership manifest maps each artifact to Djinni's configured filename. When the
type lists are omitted, native rules use the generated tree and compile a unity
source. Java packages the tree's listed files into a source JAR, and TypeScript
copies its module file from the tree. The optional compiled Swift macro requires
type lists to preserve Swift's per-file access rules.

`types` is optional and checked when supplied. It maps declaration categories
to lists of names and describes every internal declaration in the root IDL's
resolved import closure, including imported declarations. A typo or stale list
fails in Djinni's declaration action before the language codegen runs. The same
category-to-name map is carried on `DjinniInfo`.

```python
load("@djinni//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library")

djinni_library(
    name = "messages",
    idl = "messages.djinni",
    srcs = ["common.djinni"],
    types = {
        "enums": ["message_kind"],
        "records": ["message", "common_message"],
        "interfaces": ["message_listener"],
    },
)

cc_djinni_library(
    name = "messages_cc",
    deps = [":messages"],
    cpp_namespace = "example",
    visibility = ["//visibility:public"],
)

java_djinni_library(
    name = "messages_java",
    deps = [":messages"],
    java_package = "example",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "app",
    srcs = ["main.cc"],
    deps = [":messages_cc"],
)
```

### `djinni_library`

- `idl`: required single root `.djinni` label.
- `project_name`: optional consumer project name carried in `DjinniInfo` for
  wrappers that derive package names, namespaces, and output paths from it.
- `srcs`: additional imported `.djinni`, YAML, or proto file labels.
- `idl_include_files`: file labels whose parent directories are import search
  roots. These files are also declared action inputs. Use this for external
  repository or generated imports without hardcoding execution-root paths.
- `deps`: other `djinni_library` targets contributing transitive import inputs
  and generated YAML extern files.
- `types`: optional map from `enums`, `records`, and `interfaces` to lists of
  internal declaration names in the resolved IDL import closure. These names
  also determine the declaration source artifacts exposed by native and Swift
  language macros. When the map is supplied, Djinni verifies every category
  against the resolved declarations; omitted categories are treated as empty.
- `idl_include_paths`: repository-relative search directories. For example,
  `schemas/vendor` refers to that directory in the repository defining this rule,
  including when loaded as `@djinni`. Absolute paths and `..` are rejected.
  Djinni also searches relative to each importing file.
- `verify`: defaults to `True`; parsing and type resolution produce the
  declaration manifest and generated YAML extern files. Set to `False` to omit
  the input-list validation output; YAML generation and type resolution still
  run.
- `compiler`: executable label, defaulting to Djinni's `//src:djinni`, resolved
  relative to the rules repository and built in the execution configuration.

`DjinniInfo` carries the root IDL, the category-to-name `types` map, project
name, a depset of transitive source files, execution-root include paths, the
declaration and output manifests, and generated YAML trees from this target and
its dependency closure. The output ownership manifest is created for each
language action and records backend, source/header role, declaration kind/name,
and generated path. The provider does not need to duplicate language-specific
filename rules.
The YAML tree is also available through the `yaml_tree` output group.

Imports must be declared through `srcs`, `idl_include_files`, or `deps`; string
include paths do not make files action inputs. No host filesystem scanning
occurs during analysis. A dependent library can use `@extern "Type.yaml"` to
reference YAML generated by one of its `deps`. For `@extern "types.djinni"`,
Djinni loads the companion `types.djinni.yaml`; declare both files as build
inputs, such as through `srcs` or `idl_include_files`.

### Language Macros

| Macro | Generated consumer | Source handling |
| --- | --- | --- |
| `cc_djinni_library` | `cc_library` | Per-type sources with type lists; unity source otherwise |
| `c_djinni_library` | `cc_library` | Per-type sources with type lists; unity source otherwise |
| `java_djinni_library` | `java_library` | Source JAR from the generated tree |
| `jni_djinni_library` | `cc_library` | Per-type sources with type lists; unity source otherwise |
| `objc_djinni_library` | `objc_library` | Per-type sources with type lists; unity source otherwise |
| `wasm_djinni_cc_library` | `cc_library` | Per-type sources with type lists; unity source otherwise |
| `wasm_djinni_cc_binary` | `cc_binary` | Per-type sources with type lists; unity source otherwise |
| `ts_djinni_filegroup` | `filegroup` | Module file from the generated tree |
| `swift_djinni_library` (optional Apple module) | Swift and Swift/C++ libraries | Per-type source files; requires type lists |

Each wrapper accepts exactly one base target in `deps` and Djinni options as
keyword arguments. Boolean CLI options accept Starlark booleans; omission
preserves the compiler's default.

The C, ObjC, and WASM library wrappers compile their generated sources. The TS
wrapper exposes generated artifacts as a filegroup; it does not add a compiler or
consumer dependency:

```python
c_djinni_library(
    name = "messages_c",
    deps = [":messages"],
    cc_deps = ["//support-lib:djinni-support-common"],
)

objc_djinni_library(
    name = "messages_objc",
    deps = [":messages"],
    objc_deps = ["//support-lib:djinni-support-objc"],
)

wasm_djinni_cc_library(
    name = "messages_wasm",
    deps = [":messages"],
    cc_deps = ["//support-lib:djinni-support-wasm"],
)

ts_djinni_filegroup(
    name = "messages_ts",
    deps = [":messages"],
    ts_module = "messages",
)
```

The prologue options `jni_function_prologue_file` and
`objcpp_function_prologue_file` take emitted include strings, not file labels.
The generated consumers must declare the compilation dependency providing that
header. C backend options and `*_legacy_records` require a compiler revision
that supports those CLI flags; omission does not enable them.

Use `cc_deps`, `objc_deps`, or `java_deps` for compilation dependencies, including generated
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
WASM bridge class/file names use `ident_wasm_class` and `ident_wasm_file`, and
their C++ namespace uses `wasm_cpp_namespace`. Set them to match your WASM output
manifest. When omitted, these settings inherit `ident_jni_class`,
`ident_jni_file`, and `jni_namespace` for compatibility with existing builds.
Supply C++ and JNI support libraries
through `cc_deps`; for desktop JNI use `@djinni//support-lib:djinni-support-jni`.
The WASM macro produces bridge C++ sources; select an Emscripten toolchain through
your workspace's WASM build setup.

## Low-Level Codegen

`djinni_codegen` is a tree-only interface for custom integrations. Use the typed
language wrappers above for compiled libraries. Set `tree_languages` to the
backends whose generated trees the target needs.

```python
load("@djinni//bzl:djinni_codegen.bzl", "djinni_codegen")

djinni_codegen(
    name = "messages_ts",
    djinni = ":messages",
    tree_languages = ["ts"],
    ts_module = "messages",
)
```

Use either `djinni` (a `DjinniInfo` target) or an `idl` with explicit `srcs` and
`idl_include_paths`. `tree_languages` is required. Consumers can request each
language tree, the generated output inventory (`manifest`), or the per
declaration ownership inventory (`outputs_by_declaration`) through output
groups. `all` contains all declared trees and manifests.

Actions use declared files, `ctx.actions.args`, and the compiler's
`FilesToRunProvider` so its runfiles are included. Shell generation scripts are
not invoked by the rules. Missing declared outputs fail the build; detection of
extra undeclared outputs and a general `djinni_manifest_test` remain future work.

YAML type manifests imported with `@extern` remain ordinary source inputs. The
Bazel rules do not generate them; create and maintain them separately.

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

The separate `external-test` module exercises external rule loads, imported
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

The existing Swift consumers and non-Bazel scripts remain unchanged. These
rules do not yet provide Swift library macros.
