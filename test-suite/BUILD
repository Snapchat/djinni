load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary", "objc_library")
load("@rules_java//java:defs.bzl", "java_test")
load("@build_bazel_rules_apple//apple:macos.bzl", "macos_unit_test")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("//bzl:djinni_codegen.bzl", "djinni_codegen")
load(":djinni_outputs.bzl", "TESTSUITE_MAIN_DJINNI_OUTS")

djinni_codegen(
    name = "testsuite-main-djinni-codegen",
    outs = TESTSUITE_MAIN_DJINNI_OUTS,
    cpp_extended_record_include_prefix = "../../handwritten-src/cpp/",
    cpp_namespace = "testsuite",
    cpp_optional_header = "\"../../handwritten-src/cpp/optional.hpp\"",
    cpp_optional_template = "std::experimental::optional",
    ident_cpp_enum_type = "foo_bar",
    ident_java_field = "mFooBar",
    ident_jni_class = "NativeFooBar",
    ident_jni_file = "NativeFooBar",
    idl = "djinni/all.djinni",
    idl_include_paths = ["test-suite/djinni/vendor"],
    java_implement_android_os_parcelable = True,
    java_nonnull_annotation = "javax.annotation.Nonnull",
    java_nullable_annotation = "javax.annotation.CheckForNull",
    java_package = "com.dropbox.djinni.test",
    java_use_final_for_record = False,
    jni_use_on_load_initializer = False,
    objc_type_prefix = "DB",
    srcs = glob([
        "djinni/**/*.djinni",
        "djinni/**/*.yaml",
    ]) + [
        "//support-lib:dataref.yaml",
        "//support-lib:dataview.yaml",
        "//support-lib:future.yaml",
        "//support-lib:outcome.yaml",
    ],
    ts_module = "test",
    visibility = ["//visibility:public"],
    wasm_namespace = "testsuite",
    yaml_out_file = "yaml-test.yaml",
    yaml_prefix = "test_",
)

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
    copts = [
        "-fexceptions",
    ],
    deps = [
        "//support-lib:djinni-support-common",
        "@com_google_protobuf//:protobuf"
    ],
    linkstatic = True,
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
    copts = [
        "-ObjC++",
        "-std=c++17",
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
        ":djinni-tests-jni-static",
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
        "@maven_djinni//:junit_junit",
        "@maven_djinni//:io_reactivex_rxjava2_rxjava",
        "@com_google_protobuf//java/core:core",
    ],
    jvm_flags = ["-Ddjinni.native_libs_dirs=./test-suite/libdjinni-tests-jni.so", "-Xcheck:jni"],
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
    "-s WASM_BIGINT=1", # We need to pass int64_t
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
    deps = ["//support-lib:djinni-support-ts"],
    data = glob(["handwritten-src/ts/*.html",
                 "handwritten-src/ts/*.ts",
                 "handwritten-src/ts/*.json",
                 "djinni/vendor/third-party/proto/ts/*.ts",
                 "node_modules/**",
                 "generated-src/ts/*.ts"]) + [":wasm"]
)

sh_binary(
    name = "server-js",
    srcs = ["handwritten-src/js/run.sh"],
    data = glob(["handwritten-src/js/*.html",
                 "handwritten-src/js/*.js"]) + [":wasm"]
)
