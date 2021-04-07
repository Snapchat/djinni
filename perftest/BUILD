load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kt_android_library")

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
