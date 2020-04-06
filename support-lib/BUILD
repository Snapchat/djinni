load("//:toolchain.bzl", "COPTS")

cc_library(
    name = "djinni-support-jni",
    srcs = glob(["jni/*.cpp"]),
    hdrs = glob([
        "jni/*.hpp",
        "*.hpp",
    ]),
    copts = COPTS,
    includes = ["jni"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = ["//:jni_headers"],
    alwayslink = 1,
)

objc_library(
    name = "djinni-support-objc",
    srcs = glob(["objc/*.mm"]),
    hdrs = glob([
        "objc/*.h",
        "*.hpp",
    ]),
    copts = COPTS,
    includes = ["objc"],
    visibility = ["//visibility:public"],
)

java_library(
    name = "djinni-support-java",
    srcs = glob(["java/**/*.java"]),
    visibility = ["//visibility:public"],
)
