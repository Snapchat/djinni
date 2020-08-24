load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary", "objc_library")
load("@rules_java//java:defs.bzl", "java_test")
load("@build_bazel_rules_apple//apple:macos.bzl", "macos_unit_test")

cc_library(
    name = "djinni-tests-common",
    srcs = glob([
        "generated-src/cpp/*.cpp",
        "handwritten-src/cpp/*.cpp",
        "djinni/vendor/third-party/proto/cpp/*.cc",
    ]),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
        "djinni/vendor/third-party/proto/cpp/*.h",
    ]),
    includes = [
        "generated-src/cpp",
        "handwritten-src/cpp",
        "djinni/vendor/third-party",
    ],
    deps = [
        "//support-lib:djinni-support-common",
        "@com_google_protobuf//:protobuf"
    ],
    linkstatic = True,
    alwayslink = 1,
)

cc_library(
    name = "djinni-tests-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    includes = ["generated-src/jni"],
    linkstatic = True,
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-jni",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "libdjinni-tests-jni.so",
    linkshared = 1,
    deps = [":djinni-tests-jni"],
)

objc_library(
    name = "djinni-tests-objcxx",
    copts = [
        "-ObjC++",
    ],
    srcs = glob([
        "generated-src/objc/**/*.mm",
        "handwritten-src/objc/**/*.mm",
    ]),
    hdrs = glob([
        "generated-src/objc/**/*.h",
        "handwritten-src/objc/**/*.h",
        "djinni/vendor/third-party/proto/objc/*.h",
    ]),
    includes = [
        "generated-src/objc",
        "handwritten-src/objc/impl",
    ],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-objc",
        "@com_google_protobuf//:protobuf_objc",
    ],
)

objc_library(
    name = "djinni-tests-objc",
    srcs = glob([
        "generated-src/objc/**/*.m",
        "handwritten-src/objc/**/*.m",
    ]),
    hdrs = glob([
        "generated-src/objc/**/*.h",
        "handwritten-src/objc/**/*.h",
        "djinni/vendor/third-party/proto/objc/*.h",
    ]),
    includes = [
        "generated-src/objc",
        "handwritten-src/objc/impl",
    ],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-objc",
        "@com_google_protobuf//:protobuf_objc",
    ],
)

objc_library(
    name = "djinni-tests-objc-proto",
    copts = [
        "-fno-objc-arc",
    ],
    srcs = glob([
        "djinni/vendor/third-party/proto/objc/*.m",
    ]),
    hdrs = glob([
        "djinni/vendor/third-party/proto/objc/*.h",
    ]),
    deps = [
        "@com_google_protobuf//:protobuf_objc",
    ],
)

java_test(
    name = "djinni-java-tests",
    srcs = glob([
        "generated-src/java/**/*.java",
        "handwritten-src/java/**/*.java",
        "djinni/vendor/third-party/proto/java/**/*.java",
    ]),
    main_class = "com.dropbox.djinni.test.AllTests",
    use_testrunner = False,
    runtime_deps = [":libdjinni-tests-jni.so"],
    deps = [
        ":djinni-tests-jni",
        "//support-lib:djinni-support-java",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:junit_junit",
        "@com_google_protobuf//java/core:core",
    ],
)

macos_unit_test(
    name = "djinni-objc-tests",
    minimum_os_version = "10.10",
    deps = [
        "djinni-tests-objc",
        "djinni-tests-objcxx",
        "djinni-tests-objc-proto",
    ],
)
