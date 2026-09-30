load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kt_android_library")
load("//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library", "jni_djinni_library", "wasm_djinni_cc_binary")
load(":djinni_outputs.bzl", "BENCHMARK_DJINNI_OUTS")

djinni_library(
    name = "benchmark-djinni",
    srcs = [
        "//support-lib:dataref.yaml",
        "//support-lib:dataview.yaml",
    ],
    idl = "djinni_perf_benchmark.djinni",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "benchmark-common",
    srcs = glob([
        "generated-src/cpp/*.cpp",
        "handwritten-src/cpp/*.cpp",
    ]),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
    ]),
    includes = [
        "generated-src/cpp",
        "handwritten-src/cpp",
    ],
    deps = [
        "//support-lib:djinni-support-common",
    ],
    alwayslink = 1,
)

cc_djinni_library(
    name = "benchmark-common-generated",
    srcs = glob([
        "handwritten-src/cpp/*.cpp",
    ]),
    outs = BENCHMARK_DJINNI_OUTS,
    hdrs = glob([
        "handwritten-src/cpp/*.hpp",
    ]),
    cc_deps = [
        "//support-lib:djinni-support-common",
    ],
    cpp_namespace = "snapchat::djinni::benchmark",
    ident_cpp_enum_type = "foo_bar",
    includes = [
        "bazel-generated/benchmark/cpp",
        "handwritten-src/cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [":benchmark-djinni"],
    alwayslink = 1,
)

# ------------------------------------------------------------------

java_library(
    name = "benchmark-java",
    srcs = glob(["generated-src/java/**/*.java"]),
    deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
)

java_djinni_library(
    name = "benchmark-java-generated",
    outs = BENCHMARK_DJINNI_OUTS,
    ident_java_field = "mFooBar",
    java_class_access_modifier = "package",
    java_deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
    java_nonnull_annotation = "javax.annotation.Nonnull",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_package = "com.snapchat.djinni.benchmark",
    visibility = ["//visibility:public"],
    deps = [":benchmark-djinni"],
)

cc_library(
    name = "benchmark-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    includes = ["generated-src/jni"],
    linkopts = [
        "-lm",
        "-ldl",
    ],
    deps = [
        ":benchmark-common",
        "//support-lib:djinni-support-android",
    ],
    alwayslink = 1,
)

jni_djinni_library(
    name = "benchmark-jni-generated",
    outs = BENCHMARK_DJINNI_OUTS,
    cc_deps = [
        ":benchmark-common-generated",
        "//support-lib:djinni-support-jni",
    ],
    cpp_namespace = "snapchat::djinni::benchmark",
    ident_cpp_enum_type = "foo_bar",
    ident_java_field = "mFooBar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    includes = ["bazel-generated/benchmark/jni"],
    java_package = "com.snapchat.djinni.benchmark",
    linkopts = [
        "-lm",
        "-ldl",
    ],
    visibility = ["//visibility:public"],
    deps = [":benchmark-djinni"],
    alwayslink = 1,
)

# ------------------------------------------------------------------

kt_android_library(
    name = "benchmark-activity",
    srcs = glob(["android/app/src/main/java/com/snapchat/djinni/benchmark/*.kt"]),
    custom_package = "com.snapchat.djinni.benchmark",
    manifest = "android/app/src/main/AndroidManifest.xml",
    resource_files = glob(["android/app/src/main/res/**"]),
    deps = [":benchmark-java"],
)

android_binary(
    name = "perftest",
    custom_package = "com.snapchat.djinni.benchmark",
    manifest = "android/AndroidManifest.xml",
    deps = [
        ":benchmark-activity",
        ":benchmark-jni",
    ],
)

# ------------------------------------------------------------------

EMSCRIPTEN_LINKOPTS = [
    "--bind",  # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "-s MALLOC=emmalloc",  # Switch to using the much smaller implementation
    "-s MODULARIZE=1",  # Allows us to manually invoke the initialization of wasm
    "-s WASM_BIGINT=1",  # We need to pass int64_t
]

cc_binary(
    name = "perftest-wasm",
    srcs = glob([
        "generated-src/wasm/*.cpp",
        "generated-src/wasm/*.hpp",
    ]),
    copts = [
        "-fexceptions",
    ],
    linkopts = EMSCRIPTEN_LINKOPTS,
    deps = [
        ":benchmark-common",
        "//support-lib:djinni-support-wasm",
    ],
)

wasm_djinni_cc_binary(
    name = "perftest-wasm-generated",
    outs = BENCHMARK_DJINNI_OUTS,
    cc_deps = [
        ":benchmark-common-generated",
        "//support-lib:djinni-support-wasm",
    ],
    copts = [
        "-fexceptions",
    ],
    cpp_namespace = "snapchat::djinni::benchmark",
    ident_cpp_enum_type = "foo_bar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    linkopts = EMSCRIPTEN_LINKOPTS,
    visibility = ["//visibility:public"],
    wasm_namespace = "benchmark",
    wasm_omit_namespace_alias = True,
    deps = [":benchmark-djinni"],
)

wasm_cc_binary(
    name = "wasm",
    cc_target = ":perftest-wasm",
)

wasm_cc_binary(
    name = "wasm-generated",
    cc_target = ":perftest-wasm-generated",
    visibility = ["//visibility:public"],
)

sh_binary(
    name = "server",
    srcs = ["ts/run.sh"],
    data = glob([
        "ts/*.html",
        "ts/*.ts",
        "ts/*.json",
        "generated-src/ts/*.ts",
    ]) + [":wasm"],
    deps = ["//support-lib:djinni-support-ts"],
)
