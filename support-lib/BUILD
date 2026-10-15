load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")

exports_files([
    "dataref.yaml",
    "dataview.yaml",
    "future.yaml",
    "outcome.yaml",
    "provider.yaml",
])

cc_library(
    name = "djinni-support-common",
    srcs = glob(["cpp/*.cpp"]),
    hdrs = glob([
        "*.hpp",
        "cpp/*.hpp",
        "cpp/*.h",
    ]),
    copts = ["-fexceptions"],  # Required for Future.hpp which uses try/catch
    includes = ["cpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob(["jni/*.hpp"]),
    defines = ["DATAREF_JNI=1"],
    includes = ["jni"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)

cc_library(
    name = "djinni-support-android",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob(["jni/*.hpp"]),
    defines = ["DATAREF_JNI=1"],
    includes = ["jni"],
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
    ],
    alwayslink = 1,
)

objc_library(
    name = "djinni-support-objc",
    srcs = glob(
        [
            "objc/*.mm",
            "objc/*.cpp",
        ],
        allow_empty = True,
    ),
    hdrs = glob([
        "objc/*.h",
        "objc/*.hpp",
    ]),
    copts = [
        "-ObjC++",
        "-std=c++20",
    ],
    defines = ["DATAREF_OBJC=1"],
    includes = ["objc"],
    visibility = ["//visibility:public"],
    deps = ["//support-lib:djinni-support-common"],
)

java_library(
    name = "djinni-support-java",
    srcs = glob(["java/**/*.java"]),
    visibility = ["//visibility:public"],
    deps = [
        "@maven_djinni//:javax_inject_javax_inject",
    ],
)

cc_library(
    name = "djinni-support-wasm",
    srcs = glob(["wasm/*.cpp"]),
    hdrs = glob(["wasm/*.hpp"]),
    copts = [
        "-fexceptions",
    ],
    defines = ["DATAREF_WASM=1"],
    includes = ["wasm"],
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
    ],
)

sh_library(
    name = "djinni-support-ts",
    data = glob(["ts/*.ts"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "ts-sources",
    srcs = glob(["ts/*.ts"]),
    visibility = ["//visibility:public"],
)

# Legacy root-workspace names; external Swift consumers use @djinni_apple.
alias(
    name = "djinni-support-swift",
    actual = "@djinni_apple//:djinni-support-swift",
    visibility = ["//visibility:public"],
)

alias(
    name = "djinni-support-swiftxx",
    actual = "@djinni_apple//:djinni-support-swiftxx",
    visibility = ["//visibility:public"],
)

filegroup(
    name = "swift-sources",
    srcs = glob(["swift/*.swift"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "swiftxx-headers",
    srcs = glob(["swiftxx/*.hpp"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "djinni-support-swiftxx-core",
    srcs = glob(["swiftxx/*.cpp"]),
    hdrs = glob(["swiftxx/*.hpp"]),
    copts = ["-std=c++20"],
    includes = [
        ".",
        "swiftxx",
    ],
    visibility = ["//visibility:public"],
    deps = [":djinni-support-common"],
)

# cc_library version of djinni-support-objc to avoid apple_support
cc_library(
    name = "djinni-support-objc-cc",
    srcs = glob(
        [
            "objc/*.mm",
            "objc/*.cpp",
        ],
        allow_empty = True,
    ),
    hdrs = glob([
        "objc/*.h",
        "objc/*.hpp",
    ]),
    copts = [
        "-x objective-c++",
        "-std=c++17",
        "-fobjc-arc",
    ],
    defines = ["DATAREF_OBJC=1"],
    includes = ["objc"],
    linkopts = ["-framework Foundation"],
    visibility = ["//visibility:public"],
    deps = ["//support-lib:djinni-support-common"],
)
