load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")

cc_library(
    name = "djinni-support-common",
    hdrs = glob(["*.hpp", "cpp/*.hpp"]),
    includes = [".", "cpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]) + ["cpp/DataRef.cpp"],
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
    srcs = glob(["jni/*.cpp"]) + ["cpp/DataRef.cpp"],
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
    srcs = glob(["objc/*.mm"]) + ["cpp/DataRef.cpp"],
    hdrs = glob(["objc/*.h", "objc/*.hpp"]),
    copts = [
        "-ObjC++",
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
)

cc_library(
    name = "djinni-support-wasm",
    srcs = glob(["wasm/*.cpp"]) + ["cpp/DataRef.cpp"],
    hdrs = glob(["wasm/*.hpp"]),
    includes = ["wasm"],
    defines = ["DATAREF_WASM=1"],
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
