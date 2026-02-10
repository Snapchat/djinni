load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")

cc_library(
    name = "djinni-support-common",
    srcs = glob(["cpp/*.cpp"]),
    hdrs = glob(["*.hpp", "cpp/*.hpp", "cpp/*.h"]),
    includes = ["cpp"],
    copts = ["-fexceptions"],  # Required for Future.hpp which uses try/catch
    visibility = ["//visibility:public"],
)

cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob(["jni/*.hpp"]),
    includes = ["jni"],
    defines = ["DATAREF_JNI=1"],
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
    includes = ["jni"],
    defines = ["DATAREF_JNI=1"],
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
    ],
    alwayslink = 1,
)

objc_library(
    name = "djinni-support-objc",
    srcs = glob(["objc/*.mm", "objc/*.cpp"], allow_empty = True),
    hdrs = glob(["objc/*.h", "objc/*.hpp"]),
    copts = [
        "-ObjC++",
        "-std=c++20",
    ],
    includes = ["objc"],
    defines = ["DATAREF_OBJC=1"],
    visibility = ["//visibility:public"],
    deps = ["//support-lib:djinni-support-common"],
)

java_library(
    name = "djinni-support-java",
    srcs = glob(["java/**/*.java"]),
    visibility = ["//visibility:public"],
    deps = [
        "@maven_djinni//:com_google_guava_guava",
    ],
)

cc_library(
    name = "djinni-support-wasm",
    srcs = glob(["wasm/*.cpp"]),
    hdrs = glob(["wasm/*.hpp"]),
    includes = ["wasm"],
    defines = ["DATAREF_WASM=1"],
    visibility = ["//visibility:public"],
    copts = [
        "-fexceptions",
    ],
    deps = [
        "//support-lib:djinni-support-common",
    ],
)

sh_library(
    name = "djinni-support-ts",
    data = glob(["ts/*.ts"]),
    visibility = ["//visibility:public"],
)

swift_library(
    name = "djinni-support-swift",
    module_name = "DjinniSupport",
    srcs = glob(["swift/*.swift"]),
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc", "-std=c++20",
    ],
    deps = [
        ":djinni-support-swiftxx",
        "@apple_swift_protobuf//:swift-protobuf"
    ],
    visibility = ["//visibility:public"],
)

swift_interop_hint(
    name = "djinni-support-swiftxx_hint",
    module_name = "DjinniSupportCxx",
)

cc_library(
    name = "djinni-support-swiftxx",
    srcs = glob(["swiftxx/*.cpp"]),
    hdrs = glob(["swiftxx/*.hpp"]),
    includes = [
        "swiftxx",
        ".",
    ],
    deps = [
        ":djinni-support-common",
    ],
    copts = [
        "-std=c++20",
    ],
    aspect_hints = [":djinni-support-swiftxx_hint"],
    visibility = ["//visibility:public"],
)

# cc_library version of djinni-support-objc to avoid apple_support
cc_library(
    name = "djinni-support-objc-cc",
    srcs = glob(["objc/*.mm", "objc/*.cpp"], allow_empty = True),
    hdrs = glob(["objc/*.h", "objc/*.hpp"]),
    copts = [
        "-x objective-c++",
        "-std=c++17",
        "-fobjc-arc",
    ],
    includes = ["objc"],
    defines = ["DATAREF_OBJC=1"],
    linkopts = ["-framework Foundation"],
    visibility = ["//visibility:public"],
    deps = ["//support-lib:djinni-support-common"],
)
