load("@djinni//bzl:djinni_codegen.bzl", "cc_djinni_library", "djinni_library", "java_djinni_library")
load("@rules_cc//cc:defs.bzl", "cc_test")
load("@rules_java//java:defs.bzl", "java_test")

djinni_library(
    name = "schema",
    testonly = True,
    idl = "schema.djinni",
    deps = ["@djinni//bzl/tests:shared-schema"],
)

OUTS = {
    "cpp_hdrs": [
        "generated/cpp/message.hpp",
        "generated/cpp/shared.hpp",
        "generated/cpp/status.hpp",
    ],
    "java_srcs": [
        "generated/java/Message.java",
        "generated/java/Shared.java",
        "generated/java/Status.java",
    ],
}

cc_djinni_library(
    name = "schema_cc",
    testonly = True,
    outs = OUTS,
    cpp_namespace = "consumer",
    includes = ["generated/cpp"],
    library_kwargs = {"defines": ["DJINNI_CONSUMER_TEST=1"]},
    deps = [":schema"],
)

java_djinni_library(
    name = "schema_java",
    testonly = True,
    outs = OUTS,
    java_package = "consumer",
    java_use_final_for_record = False,
    deps = [":schema"],
)

cc_test(
    name = "cpp_consumer_test",
    size = "small",
    srcs = ["consumer_test.cc"],
    deps = [
        ":schema_cc",
        "@djinni//support-lib:djinni-support-common",
    ],
)

java_test(
    name = "java_consumer_test",
    size = "small",
    srcs = ["ConsumerTest.java"],
    main_class = "consumer.ConsumerTest",
    use_testrunner = False,
    deps = [":schema_java"],
)

sh_test(
    name = "cli_equivalence_test",
    size = "small",
    srcs = ["cli_equivalence_test.sh"],
    args = [
        "$(rootpath @djinni//src:djinni)",
        "$(rootpath @djinni//bzl/tests:idl/shared.djinni)",
    ],
    data = [
        "schema.djinni",
        "@djinni//bzl/tests:idl/nested.djinni",
        "@djinni//bzl/tests:idl/shared.djinni",
        "@djinni//src:djinni",
    ] + OUTS["cpp_hdrs"] + OUTS["java_srcs"],
)

test_suite(
    name = "consumer_tests",
    tests = [
        ":cli_equivalence_test",
        ":cpp_consumer_test",
        ":java_consumer_test",
    ],
)

filegroup(
    name = "runtime_support_verification",
    srcs = [
        "@djinni//support-lib:djinni-support-common",
        "@djinni//support-lib:djinni-support-java",
        "@djinni//support-lib:djinni-support-jni",
    ],
)
