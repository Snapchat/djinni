cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob([
        "jni/*.hpp",
        "*.hpp",
    ]),
    includes = ["jni"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = ["@bazel_tools//tools/jdk:jni",],
    alwayslink = 1,
)

objc_library(
    name = "djinni-support-objc",
    srcs = glob(["objc/*.mm"]),
    hdrs = glob([
        "objc/*.h",
        "*.hpp",
    ]),
    includes = ["objc"],
    visibility = ["//visibility:public"],
)

java_library(
    name = "djinni-support-java",
    srcs = glob(["java/**/*.java"]),
    visibility = ["//visibility:public"],
)
