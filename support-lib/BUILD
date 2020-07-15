load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")

cc_library(
    name = "djinni-support-common",
    hdrs = glob(["*.hpp", "cpp/*.hpp"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob(["jni/*.hpp"]),
    includes = ["jni"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        "//support-lib:djinni-support-common",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)

objc_library(
    name = "djinni-support-objc",
    srcs = glob(["objc/*.mm"]),
    hdrs = glob(["objc/*.h"]),
    copts = [
        "-ObjC++",
    ],
    includes = ["objc"],
    visibility = ["//visibility:public"],
    deps = ["//support-lib:djinni-support-common"],
)

java_library(
    name = "djinni-support-java",
    srcs = glob(["java/**/*.java"]),
    visibility = ["//visibility:public"],
)
