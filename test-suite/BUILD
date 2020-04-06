load("//:toolchain.bzl", "COPTS")
load("@build_bazel_rules_apple//apple:macos.bzl", "macos_unit_test")

cc_library(
    name = "djinni-tests-common",
    srcs = glob([
        "generated-src/cpp/*.cpp",
        "handwritten-src/cpp/*.cpp",
    ]),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
    ]),
    copts = COPTS,
    includes = [
        "generated-src/cpp",
        "handwritten-src/cpp",
    ],
    linkstatic = True,
    alwayslink = 1,
)

cc_library(
    name = "djinni-tests-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    copts = COPTS,
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
    copts = COPTS,
    linkshared = 1,
    deps = [":djinni-tests-jni"],
)

objc_library(
    name = "djinni-tests-objcxx",
    srcs = glob([
        "generated-src/objc/**/*.mm",
        "handwritten-src/objc/**/*.mm",
    ]),
    hdrs = glob([
        "generated-src/objc/**/*.h",
        "handwritten-src/objc/**/*.h",
    ]),
    copts = COPTS,
    includes = [
        "generated-src/objc",
        "handwritten-src/objc/impl",
    ],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-objc",
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
    ]),
    includes = [
        "generated-src/objc",
        "handwritten-src/objc/impl",
    ],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-objc",
    ],
)

java_test(
    name = "djinni-java-tests",
    srcs = glob([
        "generated-src/java/**/*.java",
        "handwritten-src/java/**/*.java",
    ]),
    main_class = "com.dropbox.djinni.test.AllTests",
    use_testrunner = False,
    runtime_deps = [
        ":libdjinni-tests-jni.so",
    ],
    deps = [
        ":djinni-tests-jni",
        "//support-lib:djinni-support-java",
        "@jsr305//jar",
        "@junit//jar",
    ],
)

macos_unit_test(
    name = "djinni-objc-tests",
    minimum_os_version = "10.10",
    deps = ["djinni-tests-objc", "djinni-tests-objcxx"],
)
