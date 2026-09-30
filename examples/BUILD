load("@build_bazel_rules_apple//apple:ios.bzl", "ios_application")
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library", "jni_djinni_library", "wasm_djinni_cc_binary")
load(":djinni_outputs.bzl", "EXAMPLE_DJINNI_OUTS")

djinni_library(
    name = "example-djinni",
    idl = "example.djinni",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "textsort-common",
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

android_library(
    name = "textsort-activity",
    srcs = glob(["handwritten-src/java/com/dropbox/textsort/*.java"]),
    custom_package = "com.dropbox.textsort",
    manifest = "android/app/src/main/AndroidManifest.xml",
    resource_files = glob(
        ["android/app/src/main/res/**"],
        exclude = ["android/app/src/main/res/.DS_Store"],
    ),
    deps = [":textsort-java"],
)

android_binary(
    name = "android-app",
    custom_package = "com.dropbox.textsort",
    manifest = "android/AndroidManifest.xml",
    deps = [
        ":textsort-activity",
        ":textsort-jni",
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
    name = "textsort-wasm",
    srcs = glob([
        "generated-src/wasm/*.cpp",
        "generated-src/wasm/*.hpp",
    ]),
    copts = [
        "-fexceptions",
    ],
    linkopts = EMSCRIPTEN_LINKOPTS,
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

wasm_cc_binary(
    name = "wasm",
    cc_target = ":textsort-wasm",
)

wasm_cc_binary(
    name = "wasm-generated",
    cc_target = ":textsort-wasm-generated",
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
)
