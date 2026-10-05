load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")
load("@rules_cc//cc:defs.bzl", "cc_library")

exports_files(["swift_protobuf.BUILD"])

swift_interop_hint(
    name = "swiftxx_hint",
    module_name = "DjinniSupportCxx",
)

cc_library(
    name = "djinni-support-swiftxx",
    hdrs = ["@djinni_rules//support-lib:swiftxx-headers"],
    aspect_hints = [":swiftxx_hint"],
    visibility = ["//visibility:public"],
    deps = ["@djinni_rules//support-lib:djinni-support-swiftxx-core"],
)

swift_library(
    name = "djinni-support-swift",
    srcs = ["@djinni_rules//support-lib:swift-sources"],
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc",
        "-std=c++20",
    ],
    module_name = "DjinniSupport",
    visibility = ["//visibility:public"],
    deps = [
        ":djinni-support-swiftxx",
        "@apple_swift_protobuf//:swift-protobuf",
    ],
)
