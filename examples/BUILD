load("//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library", "jni_djinni_library", "wasm_djinni_cc_binary")
load(":djinni_outputs.bzl", "EXAMPLE_DJINNI_OUTS")

djinni_library(
    name = "example-djinni",
    idl = "example.djinni",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "textsort-common",
    srcs = glob(
        [
            "generated-src/cpp/*.cpp",
            "handwritten-src/cpp/*.cpp",
        ],
        allow_empty = True,
    ),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
    ]),
    includes = [
        "generated-src/cpp",
        "handwritten-src/cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
    ],
    alwayslink = 1,
)

cc_djinni_library(
    name = "textsort-common-generated",
    srcs = glob([
        "handwritten-src/cpp/*.cpp",
    ]),
    outs = EXAMPLE_DJINNI_OUTS,
    hdrs = glob([
        "handwritten-src/cpp/*.hpp",
    ]),
    cc_deps = [
        "//support-lib:djinni-support-common",
    ],
    cpp_namespace = "textsort",
    ident_cpp_enum_type = "foo_bar",
    includes = [
        "bazel-generated/example/cpp",
        "handwritten-src/cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [":example-djinni"],
    alwayslink = 1,
)

# ------------------------------------------------------------------

java_library(
    name = "textsort-java",
    srcs = glob(["generated-src/java/**/*.java"]),
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
)

java_djinni_library(
    name = "textsort-java-generated",
    outs = EXAMPLE_DJINNI_OUTS,
    ident_java_field = "mFooBar",
    java_class_access_modifier = "package",
    java_deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
    java_nonnull_annotation = "javax.annotation.Nonnull",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_package = "com.dropbox.textsort",
    visibility = ["//visibility:public"],
    deps = [":example-djinni"],
)

cc_library(
    name = "textsort-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    includes = ["generated-src/jni"],
    linkopts = [
        "-lm",
        "-ldl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":textsort-common",
        "//support-lib:djinni-support-android",
    ],
    alwayslink = 1,
)

jni_djinni_library(
    name = "textsort-jni-generated",
    outs = EXAMPLE_DJINNI_OUTS,
    cc_deps = [
        ":textsort-common-generated",
        "//support-lib:djinni-support-jni",
    ],
    cpp_namespace = "textsort",
    ident_cpp_enum_type = "foo_bar",
    ident_java_field = "mFooBar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    includes = ["bazel-generated/example/jni"],
    java_package = "com.dropbox.textsort",
    linkopts = [
        "-lm",
        "-ldl",
    ],
    visibility = ["//visibility:public"],
    deps = [":example-djinni"],
    alwayslink = 1,
)

# ------------------------------------------------------------------

EMSCRIPTEN_LINKOPTS = [
    "--bind",  # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "-s MALLOC=emmalloc",  # Switch to using the much smaller implementation
    "-s MODULARIZE=1",  # Allows us to manually invoke the initialization of wasm
    "-s WASM_BIGINT=1",  # We need to pass int64_t
    "-s EXPORTED_RUNTIME_METHODS=HEAPU8,HEAP8,HEAPU16,HEAP16,HEAPU32,HEAP32,HEAPF32,HEAPF64",  # Export heap views for memory access (required in Emscripten 4.x)
]

cc_binary(
    name = "textsort-wasm",
    srcs = glob([
        "generated-src/wasm/*.cpp",
        "generated-src/wasm/*.hpp",
    ]),
    copts = [
        "-fexceptions",
    ],
    linkopts = EMSCRIPTEN_LINKOPTS,
    visibility = ["//visibility:public"],
    deps = [
        ":textsort-common",
        "//support-lib:djinni-support-wasm",
    ],
)

wasm_djinni_cc_binary(
    name = "textsort-wasm-generated",
    outs = EXAMPLE_DJINNI_OUTS,
    cc_deps = [
        ":textsort-common-generated",
        "//support-lib:djinni-support-wasm",
    ],
    copts = [
        "-fexceptions",
    ],
    cpp_namespace = "textsort",
    ident_cpp_enum_type = "foo_bar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    linkopts = EMSCRIPTEN_LINKOPTS,
    visibility = ["//visibility:public"],
    deps = [":example-djinni"],
)

filegroup(
    name = "activity-sources",
    srcs = glob(["handwritten-src/java/com/dropbox/textsort/*.java"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "swift-sources",
    srcs = glob(
        ["generated-src/swift/*.swift"],
        exclude = ["generated-src/swift/*+Private.swift"],
    ),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "swift-bridge-sources",
    srcs = glob(["generated-src/swift/*+Private.swift"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "swiftxx-headers",
    srcs = glob(["generated-src/swiftxx/*.hpp"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "textsort-swiftxx-core",
    srcs = glob(["generated-src/swiftxx/*.cpp"]),
    hdrs = glob(["generated-src/swiftxx/*.hpp"]),
    copts = ["-std=c++20"],
    includes = ["generated-src/swiftxx"],
    visibility = ["//visibility:public"],
    deps = [
        ":textsort-common",
        "//support-lib:djinni-support-swiftxx-core",
    ],
)

filegroup(
    name = "web-sources",
    srcs = glob([
        "ts/*.html",
        "ts/*.ts",
        "ts/*.json",
        "generated-src/ts/*.ts",
    ]),
    visibility = ["//visibility:public"],
)

exports_files(["ts/run.sh"])
