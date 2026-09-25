# Djinni Bazel Code Generation Rules

This document describes the planned Bazel rules for running Djinni code generation during a Bazel build. The first implementation should focus on rule documentation and example BUILD usage, then add the Starlark rules once the API is agreed on.

## Goals

- Run Djinni as a normal Bazel action instead of using `run_djinni.sh` scripts.
- Generate C++, JNI, Objective-C, Objective-C++, Java, TypeScript, WASM bridge, and YAML outputs under `bazel-out`.
- Let generated files feed directly into normal Bazel rules such as `cc_library`, `java_library`, `objc_library`, and `sh_binary`.
- Keep generated output declarations explicit and reviewable.
- Provide a manifest verifier so output lists stay in sync with Djinni IDL changes.

## Non-Goals

- The first version does not replace all existing checked-in generated files.
- The first version does not infer every imported file automatically during Bazel analysis.
- The first version does not change Djinni's Scala generator behavior unless manifest verification exposes a gap.

## Why Output Manifests Are Required

Bazel needs to know ordinary output files during analysis, before an action runs. Djinni output filenames depend on the parsed IDL, language-specific options, identifier styles, and generated type names. That means a Bazel rule cannot run Djinni, discover arbitrary file names, and then feed those files into `cc_library` or `java_library` in the same analysis phase.

Djinni already supports:

- `--list-in-files`
- `--list-out-files`
- `--skip-generation`

The planned rules should use checked-in output manifests for declared outputs, plus a verifier that runs Djinni in manifest mode and fails when the checked-in lists are stale.

## Rule Overview

### `djinni_codegen`

Low-level rule that runs the Djinni binary and returns generated outputs grouped by language.

```python
load("//bzl:djinni_codegen.bzl", "djinni_codegen")

djinni_codegen(
    name = "example_djinni",
    idl = "example.djinni",
    srcs = [
        "example.djinni",
    ],
    generators = [
        "cpp",
        "java",
        "jni",
        "objc",
        "objcpp",
        "wasm",
        "ts",
    ],
    java_package = "com.dropbox.textsort",
    java_class_access_modifier = "package",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_nonnull_annotation = "javax.annotation.Nonnull",
    ident_java_field = "mFooBar",
    cpp_namespace = "textsort",
    ident_cpp_enum_type = "foo_bar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    objc_type_prefix = "TXS",
    objc_swift_bridging_header = "TextSort-Bridging-Header",
    ts_module = "example",
    outs = {
        "cpp": [
            "generated/example/cpp/item_list.hpp",
            "generated/example/cpp/sort_items.hpp",
            "generated/example/cpp/sort_order.hpp",
            "generated/example/cpp/textbox_listener.hpp",
        ],
        "java": [
            "generated/example/java/com/dropbox/textsort/ItemList.java",
            "generated/example/java/com/dropbox/textsort/SortItems.java",
            "generated/example/java/com/dropbox/textsort/SortOrder.java",
            "generated/example/java/com/dropbox/textsort/TextboxListener.java",
        ],
        "jni": [
            "generated/example/jni/NativeItemList.cpp",
            "generated/example/jni/NativeItemList.hpp",
            "generated/example/jni/NativeSortItems.cpp",
            "generated/example/jni/NativeSortItems.hpp",
            "generated/example/jni/NativeSortOrder.hpp",
            "generated/example/jni/NativeTextboxListener.cpp",
            "generated/example/jni/NativeTextboxListener.hpp",
        ],
        "objc": [
            "generated/example/objc/TXSItemList.h",
            "generated/example/objc/TXSItemList.mm",
            "generated/example/objc/TXSItemList+Private.h",
            "generated/example/objc/TXSItemList+Private.mm",
            "generated/example/objc/TXSSortItems.h",
            "generated/example/objc/TXSSortItems+Private.h",
            "generated/example/objc/TXSSortItems+Private.mm",
            "generated/example/objc/TXSSortOrder.h",
            "generated/example/objc/TXSSortOrder+Private.h",
            "generated/example/objc/TXSTextboxListener.h",
            "generated/example/objc/TXSTextboxListener+Private.h",
            "generated/example/objc/TXSTextboxListener+Private.mm",
            "generated/example/objc/TextSort-Bridging-Header.h",
        ],
        "wasm": [
            "generated/example/wasm/NativeItemList.cpp",
            "generated/example/wasm/NativeItemList.hpp",
            "generated/example/wasm/NativeSortItems.cpp",
            "generated/example/wasm/NativeSortItems.hpp",
            "generated/example/wasm/NativeSortOrder.cpp",
            "generated/example/wasm/NativeSortOrder.hpp",
            "generated/example/wasm/NativeTextboxListener.cpp",
            "generated/example/wasm/NativeTextboxListener.hpp",
        ],
        "ts": [
            "generated/example/ts/example.ts",
        ],
    },
)
```

The rule should expose output groups:

- `cpp_srcs`
- `cpp_hdrs`
- `jni_srcs`
- `jni_hdrs`
- `objc_srcs`
- `objc_hdrs`
- `java_srcs`
- `wasm_srcs`
- `wasm_hdrs`
- `ts_srcs`
- `yaml_srcs`
- `all`

### `djinni_manifest_test`

Verifier rule that runs Djinni with `--skip-generation true`, `--list-in-files`, and `--list-out-files`, then compares the result with checked-in manifests.

```python
load("//bzl:djinni_codegen.bzl", "djinni_manifest_test")

djinni_manifest_test(
    name = "example_djinni_manifest_test",
    idl = "example.djinni",
    srcs = [
        "example.djinni",
    ],
    generators = [
        "cpp",
        "java",
        "jni",
        "objc",
        "objcpp",
        "wasm",
        "ts",
    ],
    java_package = "com.dropbox.textsort",
    cpp_namespace = "textsort",
    objc_type_prefix = "TXS",
    ts_module = "example",
    expected_in_files = "example_djinni_inputs.txt",
    expected_out_files = "example_djinni_outputs.txt",
)
```

### Language Macros

Language macros should wrap `djinni_codegen` and native Bazel rules. They should be conveniences only; the low-level rule remains the source of truth.

```python
load("//bzl:djinni_codegen.bzl", "djinni_cc_library", "djinni_java_library")

djinni_cc_library(
    name = "textsort-common",
    idl = "example.djinni",
    srcs = ["example.djinni"],
    cpp_namespace = "textsort",
    ident_cpp_enum_type = "foo_bar",
    outs = {
        "srcs": [
        ],
        "hdrs": [
            "generated/example/cpp/item_list.hpp",
            "generated/example/cpp/sort_items.hpp",
            "generated/example/cpp/sort_order.hpp",
            "generated/example/cpp/textbox_listener.hpp",
        ],
    },
    includes = [
        "generated/example/cpp",
        "handwritten-src/cpp",
    ],
    deps = [
        "//support-lib:djinni-support-common",
    ],
)

djinni_java_library(
    name = "textsort-java",
    idl = "example.djinni",
    srcs = ["example.djinni"],
    java_package = "com.dropbox.textsort",
    java_class_access_modifier = "package",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_nonnull_annotation = "javax.annotation.Nonnull",
    ident_java_field = "mFooBar",
    outs = [
        "generated/example/java/com/dropbox/textsort/ItemList.java",
        "generated/example/java/com/dropbox/textsort/SortItems.java",
        "generated/example/java/com/dropbox/textsort/SortOrder.java",
        "generated/example/java/com/dropbox/textsort/TextboxListener.java",
    ],
    deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
)
```

## Perftest Example

This mirrors `perftest/run_djinni.sh`.

```python
load("//bzl:djinni_codegen.bzl", "djinni_codegen")
load(":benchmark_djinni_outputs.bzl", "BENCHMARK_DJINNI_OUTS")

djinni_codegen(
    name = "benchmark_djinni",
    idl = "djinni_perf_benchmark.djinni",
    srcs = ["djinni_perf_benchmark.djinni"],
    generators = [
        "cpp",
        "java",
        "jni",
        "objc",
        "objcpp",
        "wasm",
        "ts",
    ],
    java_package = "com.snapchat.djinni.benchmark",
    java_class_access_modifier = "package",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_nonnull_annotation = "javax.annotation.Nonnull",
    ident_java_field = "mFooBar",
    cpp_namespace = "snapchat::djinni::benchmark",
    ident_cpp_enum_type = "foo_bar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    objc_type_prefix = "TXS",
    objc_swift_bridging_header = "Benchmark-Bridging-Header",
    wasm_namespace = "benchmark",
    wasm_omit_namespace_alias = True,
    ts_module = "perftest",
    outs = BENCHMARK_DJINNI_OUTS,
)
```

The `outs` attribute should accept a dictionary. Large targets should prefer loading that dictionary from a `.bzl` manifest so BUILD files stay readable.

## Test Suite Example

The test suite currently runs Djinni multiple times with different IDL files and option sets. The Bazel migration should model those as separate `djinni_codegen` targets so each action has one option set.

```python
load(":testsuite_main_djinni_outputs.bzl", "TESTSUITE_MAIN_DJINNI_OUTS")

djinni_codegen(
    name = "testsuite_main_djinni",
    idl = "djinni/all.djinni",
    srcs = [
        "djinni/all.djinni",
        "djinni/common.djinni",
        "djinni/set.djinni",
        "djinni/vendor/third-party/date.djinni",
        "djinni/vendor/third-party/date.yaml",
        "djinni/vendor/third-party/duration.djinni",
        "djinni/vendor/third-party/duration.yaml",
        "djinni/vendor/third-party/outcome.djinni",
        "djinni/vendor/third-party/proto.djinni",
        "djinni/vendor/third-party/proto.yaml",
        "djinni/vendor/third-party/proto2.yaml",
        "//support-lib:future.yaml",
        "//support-lib:outcome.yaml",
        "//support-lib:dataref.yaml",
        "//support-lib:dataview.yaml",
    ],
    idl_include_paths = [
        "djinni/vendor",
    ],
    generators = [
        "cpp",
        "java",
        "jni",
        "objc",
        "objcpp",
        "wasm",
        "ts",
        "yaml",
    ],
    java_package = "com.dropbox.djinni.test",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_nonnull_annotation = "javax.annotation.Nonnull",
    java_use_final_for_record = False,
    java_implement_android_os_parcelable = True,
    cpp_namespace = "testsuite",
    cpp_optional_template = "std::experimental::optional",
    cpp_optional_header = "\"../../handwritten-src/cpp/optional.hpp\"",
    cpp_extended_record_include_prefix = "../../handwritten-src/cpp/",
    ident_cpp_enum_type = "foo_bar",
    jni_use_on_load_initializer = False,
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    objc_type_prefix = "DB",
    wasm_namespace = "testsuite",
    ts_module = "test",
    yaml_out_file = "yaml-test.yaml",
    yaml_prefix = "test_",
    outs = TESTSUITE_MAIN_DJINNI_OUTS,
)
```

## Initial Attribute Set

The first implementation should support the options currently used by `examples/run_djinni.sh`, `perftest/run_djinni.sh`, and `test-suite/run_djinni.sh`:

- `idl`
- `srcs`
- `idl_include_paths`
- `generators`
- `outs`
- `java_package`
- `java_class_access_modifier`
- `java_nullable_annotation`
- `java_nonnull_annotation`
- `java_implement_android_os_parcelable`
- `java_use_final_for_record`
- `java_gen_interface`
- `cpp_namespace`
- `cpp_optional_template`
- `cpp_optional_header`
- `cpp_extended_record_include_prefix`
- `cpp_use_wide_strings`
- `jni_use_on_load_initializer`
- `jni_function_prologue_file`
- `objc_type_prefix`
- `objc_swift_bridging_header`
- `objcpp_function_prologue_file`
- `wasm_namespace`
- `wasm_omit_namespace_alias`
- `ts_module`
- `yaml_out_file`
- `yaml_prefix`
- all currently used `ident_*` options

Additional Djinni CLI flags can be added after the first migration target proves the shape.

## Migration Strategy

1. Add rule documentation and reviewed examples.
2. Add `bzl/djinni_codegen.bzl` with `djinni_codegen` and `djinni_manifest_test`.
3. Migrate `examples` to generated-at-build-time outputs.
4. Migrate `perftest`.
5. Split the test suite into one `djinni_codegen` target per current Djinni invocation.
6. Decide whether checked-in `generated-src` should remain as golden outputs or be removed from normal build inputs.

## Open Questions

- Should manifests be handwritten `.bzl` files, plain text files generated by `--list-out-files`, or both?
- Should `djinni_codegen` use one action for all languages or one action per language?
- Should generated C++ and generated JNI live in separate output roots even when they are produced by one Djinni invocation?
- Should the first version support tree artifacts for exploratory targets that do not need direct `cc_library` or `java_library` integration?
- Should input manifests be mandatory, or should `srcs` plus `djinni_manifest_test` be enough?
