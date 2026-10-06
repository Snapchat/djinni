load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")
load("@rules_cc//cc:defs.bzl", "cc_library")

swift_library(
    name = "textsort-swift",
    srcs = ["@djinni_rules//examples:swift-sources"],
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc",
        "-std=c++20",
    ],
    module_name = "TextSort",
    visibility = ["//visibility:public"],
    deps = ["@djinni_apple//:djinni-support-swift"],
)

swift_interop_hint(
    name = "textsort-swiftxx-hint",
    module_name = "TextSortCxx",
)

cc_library(
    name = "textsort-swiftxx",
    hdrs = ["@djinni_rules//examples:swiftxx-headers"],
    aspect_hints = [":textsort-swiftxx-hint"],
    visibility = ["//visibility:public"],
    deps = [
        "@djinni_apple//:djinni-support-swiftxx",
        "@djinni_rules//examples:textsort-swiftxx-core",
    ],
)

swift_library(
    name = "textsort-swift-bridge",
    srcs = ["@djinni_rules//examples:swift-bridge-sources"],
    copts = [
        "-cxx-interoperability-mode=default",
        "-Xcc",
        "-std=c++20",
    ],
    module_name = "TextSortBridge",
    visibility = ["//visibility:public"],
    deps = [
        ":textsort-swift",
        ":textsort-swiftxx",
        "@djinni_apple//:djinni-support-swift",
    ],
)
