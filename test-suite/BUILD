load("@build_bazel_rules_apple//apple:macos.bzl", "macos_unit_test")
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library", "swift_test")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_test")

cc_library(
    name = "djinni-tests-common",
    srcs = glob(
        [
            "generated-src/cpp/*.cpp",
            "handwritten-src/cpp/*.cpp",
            "djinni/vendor/third-party/proto/cpp/*.cc",
        ],
    ),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
        "djinni/vendor/third-party/proto/cpp/*.h",
    ]),
    copts = [
        "-fexceptions",
    ],
    includes = [
        "djinni/vendor/third-party",
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
    name = "djinni-tests-c-lib",
    srcs = glob(
        [
            "generated-src/c/*.cpp",
        ],
    ),
    hdrs = glob([
        "generated-src/c/*.h",
    ]),
    includes = [
        "generated-src/c",
    ],
    deps = [
        ":djinni-tests-common",
        "@gtest",
    ] + select({
        "@platforms//os:macos": ["//support-lib:djinni-support-objc"],
    }),
    linkstatic = 1,
)

cc_test(
    name = "djinni-tests-c",
    linkstatic = 1,
    visibility = ["//visibility:public"],
    srcs = glob(
        [
            "handwritten-src/c/tests/*.cpp",
        ],
    ),
    deps = [
        ":djinni-tests-c-lib",
        "@gtest//:gtest_main",
    ],
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
        "djinni/vendor/third-party/proto/objc/*.h",
    ]),
    copts = [
        "-ObjC++",
        "-std=c++20",
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
    ], allow_empty = True),
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
    srcs = glob([
        "djinni/vendor/third-party/proto/objc/*.m",
    ]),
    hdrs = glob([
        "djinni/vendor/third-party/proto/objc/*.h",
    ]),
    copts = [
        "-fno-objc-arc",
    ],
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
        "@maven_djinni//:com_google_guava_guava",
        "@maven_djinni//:io_reactivex_rxjava2_rxjava",
        "@maven_djinni//:junit_junit",
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

EMSCRIPTEN_LINKOPTS = [
    "--bind",  # Compiles the source code using the Embind bindings to connect C/C++ and JavaScript
    "-s MALLOC=emmalloc",  # Switch to using the much smaller implementation
    "-s MODULARIZE=1",  # Allows us to manually invoke the initialization of wasm
    "-s WASM_BIGINT=1",  # We need to pass int64_t
    "-s EXPORTED_RUNTIME_METHODS=HEAPU8,HEAP8,HEAPU16,HEAP16,HEAPU32,HEAP32,HEAPF32,HEAPF64",  # Export heap views for memory access (required in Emscripten 4.x)
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
    ], allow_empty = True) + [":wasm"],
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

swift_helper_files = "handwritten-src/swift/*+Helper.swift"

swift_error_files = "handwritten-src/swift/*+Error.swift"

swift_test(
    name = "djinni-swift-tests",
    srcs = glob(
        ["handwritten-src/swift/*.swift"],
        exclude = [
            swift_helper_files,
            swift_error_files,
        ],
    ),
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc",
        "-std=c++20",
    ],
    deps = [":test-swift-bridge"],
)

swift_library(
    name = "test-swift-bridge",
    srcs = glob(
        [
            "generated-src/swift/*.swift",
            swift_error_files,
            swift_helper_files,
        ],
        exclude = [
            "generated-src/swift/*Lazy*.swift",  # Legacy - supplier types use *Supplier*.swift
        ],
    ),
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc",
        "-std=c++20",
    ],
    module_name = "TestSuite",
    visibility = ["//visibility:public"],
    deps = [
        ":djinni-tests-common",
        ":protobuf-test-swift",
        ":test-swiftxx",
        "//support-lib:djinni-support-swift",
        "//support-lib:djinni-support-swiftxx",
    ],
)

swift_library(
    name = "protobuf-test-swift",
    srcs = glob(["djinni/vendor/third-party/proto/swift/*.swift"]),
    module_name = "ProtobufTest",
    visibility = ["//visibility:public"],
    deps = [
        "@apple_swift_protobuf//:swift-protobuf",
    ],
)

swift_interop_hint(
    name = "test-swiftxx_hint",
    module_name = "TestSuiteCxx",
)

cc_library(
    name = "test-swiftxx",
    srcs = glob(
        ["generated-src/swiftxx/*.cpp"],
        exclude = ["generated-src/swiftxx/*Lazy*.cpp"],  # Legacy - supplier types use *Supplier*
    ),
    hdrs = glob(
        ["generated-src/swiftxx/*.hpp"],
        exclude = ["generated-src/swiftxx/*Lazy*.hpp"],  # Legacy - supplier types use *Supplier*
    ),
    copts = ["-std=c++20"],
    includes = [
        ".",
        "generated-src/swiftxx",
    ],
    aspect_hints = [":test-swiftxx_hint"],
    visibility = ["//visibility:public"],
    deps = [
        ":djinni-tests-common",
        "//support-lib:djinni-support-common",
        "//support-lib:djinni-support-swiftxx",
    ],
)
