load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")
load("@rules_shell//shell:sh_library.bzl", "sh_library")

cc_library(
    name = "djinni-support-common",
    srcs = glob(["cpp/*.cpp"]),
    hdrs = glob([
        "*.hpp",
        "cpp/*.hpp",
    ]),
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
    srcs = glob([
        "objc/*.mm",
        "objc/*.cpp",
    ]),
    hdrs = glob([
        "objc/*.h",
        "objc/*.hpp",
    ]),
    copts = [
        "-ObjC++",
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
