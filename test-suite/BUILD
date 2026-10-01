load("@build_bazel_rules_apple//apple:macos.bzl", "macos_unit_test")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_test")
load("@rules_shell//shell:sh_binary.bzl", "sh_binary")

genrule(
    name = "test_protobuf_codegen",
    srcs = [
        "djinni/vendor/third-party/proto/test.proto",
        "djinni/vendor/third-party/proto/test2.proto",
    ],
    outs = [
        "bazel-generated/proto/cpp/test.pb.cc",
        "bazel-generated/proto/cpp/test.pb.h",
        "bazel-generated/proto/cpp/test2.pb.cc",
        "bazel-generated/proto/cpp/test2.pb.h",
        "bazel-generated/proto/java/djinni/test/Test.java",
        "bazel-generated/proto/java/djinni/test2/Test2.java",
        "bazel-generated/proto/objc/Test.pbobjc.h",
        "bazel-generated/proto/objc/Test.pbobjc.m",
    ],
    cmd = """
        $(location @com_google_protobuf//:protoc) \
            --proto_path=test-suite/djinni/vendor/third-party/proto \
            --cpp_out=$(RULEDIR)/bazel-generated/proto/cpp \
            --java_out=$(RULEDIR)/bazel-generated/proto/java \
            $(SRCS)
        $(location @com_google_protobuf//:protoc) \
            --proto_path=test-suite/djinni/vendor/third-party/proto \
            --objc_out=$(RULEDIR)/bazel-generated/proto/objc \
            $(location djinni/vendor/third-party/proto/test.proto)
    """,
    tools = ["@com_google_protobuf//:protoc"],
)

PROTO_CPP_SRCS = [
    "bazel-generated/proto/cpp/test.pb.cc",
    "bazel-generated/proto/cpp/test2.pb.cc",
]

PROTO_CPP_HDRS = [
    "bazel-generated/proto/cpp/test.pb.h",
    "bazel-generated/proto/cpp/test2.pb.h",
]

PROTO_OBJC_HDRS = ["bazel-generated/proto/objc/Test.pbobjc.h"]

cc_library(
    name = "djinni-tests-common",
    srcs = glob([
        "generated-src/cpp/*.cpp",
        "handwritten-src/cpp/*.cpp",
    ]) + PROTO_CPP_SRCS,
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
    ]) + PROTO_CPP_HDRS,
    copts = [
        "-fexceptions",
    ],
    includes = [
        "bazel-generated",
        "generated-src/cpp",
        "handwritten-src/cpp",
    ],
    linkstatic = True,
    deps = [
        "//support-lib:djinni-support-common",
        "@com_google_protobuf//:protobuf",
    ],
    alwayslink = 1,
)

cc_library(
    name = "djinni-tests-jni-static",
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
    deps = [":djinni-tests-jni-static"],
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
    ]) + PROTO_OBJC_HDRS,
    copts = [
        "-ObjC++",
        "-std=c++17",
    ],
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
    ]) + PROTO_OBJC_HDRS,
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
    srcs = ["bazel-generated/proto/objc/Test.pbobjc.m"],
    hdrs = PROTO_OBJC_HDRS,
    copts = [
        "-fno-objc-arc",
    ],
    includes = ["bazel-generated"],
    deps = [
        "@com_google_protobuf//:protobuf_objc",
    ],
)

java_test(
    name = "djinni-java-tests",
    srcs = glob([
        "generated-src/java/**/*.java",
        "handwritten-src/java/**/*.java",
    ]) + [
        "bazel-generated/proto/java/djinni/test/Test.java",
        "bazel-generated/proto/java/djinni/test2/Test2.java",
    ],
    jvm_flags = [
        "-Ddjinni.native_libs_dirs=./test-suite/libdjinni-tests-jni.so",
        "-Xcheck:jni",
    ],
    main_class = "com.dropbox.djinni.test.AllTests",
    use_testrunner = False,
    runtime_deps = [":libdjinni-tests-jni.so"],
    deps = [
        ":djinni-tests-jni-static",
        "//support-lib:djinni-support-java",
        "@com_google_protobuf//java/core",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
        "@maven_djinni//:io_reactivex_rxjava2_rxjava",
        "@maven_djinni//:junit_junit",
    ],
)

macos_unit_test(
    name = "djinni-objc-tests",
    minimum_os_version = "10.10",
    deps = [
        "djinni-tests-objc",
        "djinni-tests-objc-proto",
        "djinni-tests-objcxx",
    ],
)

EMSCRIPTEN_LINKOPTS = [
    "--bind",  # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "-s MALLOC=emmalloc",  # Switch to using the much smaller implementation
    "-s MODULARIZE=1",  # Allows us to manually invoke the initialization of wasm
    "-s WASM_BIGINT=1",  # We need to pass int64_t
]

cc_binary(
    name = "test-wasm",
    srcs = glob([
        "generated-src/wasm/*.cpp",
        "generated-src/wasm/*.hpp",
    ]),
    copts = [
        "-fexceptions",
    ],
    linkopts = EMSCRIPTEN_LINKOPTS + ["-fexceptions"],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-wasm",
    ],
)

wasm_cc_binary(
    name = "wasm",
    cc_target = ":test-wasm",
)

sh_binary(
    name = "server-ts",
    srcs = ["handwritten-src/ts/run.sh"],
    data = glob([
        "handwritten-src/ts/*.html",
        "handwritten-src/ts/*.ts",
        "handwritten-src/ts/*.json",
        "djinni/vendor/third-party/proto/ts/*.ts",
        "node_modules/**",
        "generated-src/ts/*.ts",
    ]) + [":wasm"],
    deps = ["//support-lib:djinni-support-ts"],
)

sh_binary(
    name = "server-js",
    srcs = ["handwritten-src/js/run.sh"],
    data = glob([
        "handwritten-src/js/*.html",
        "handwritten-src/js/*.js",
    ]) + [":wasm"],
)
