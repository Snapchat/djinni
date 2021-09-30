load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kt_android_library")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")

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

# ------------------------------------------------------------------

java_library(
    name = "benchmark-java",
    srcs = glob(["generated-src/java/**/*.java"]),
    deps = [
        "//support-lib:djinni-support-java",
        "@maven//:com_google_code_findbugs_jsr305",
    ],
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
    "-s WASM_BIGINT=1", # We need to pass int64_t
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

wasm_cc_binary(
    name = "wasm",
    cc_target = ":perftest-wasm",
)

sh_binary(
    name = "server",
    srcs = ["ts/run.sh"],
    deps = ["//support-lib:djinni-support-ts"],
    data = glob(["ts/*.html", "ts/*.ts", "ts/*.json", "generated-src/ts/*.ts"]) + [":wasm"]
)
