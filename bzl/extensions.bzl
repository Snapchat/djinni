"""Bazel module extensions for Djinni"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _non_module_deps_impl(module_ctx):
    """Module extension to load non-BCR dependencies"""
    
    # Swift Protobuf (not yet in BCR)
    swiftprotobuf_version = "1.28.2"
    http_archive(
        name = "apple_swift_protobuf",
        url = "https://github.com/apple/swift-protobuf/archive/refs/tags/{}.tar.gz".format(swiftprotobuf_version),
        strip_prefix = "swift-protobuf-{}".format(swiftprotobuf_version),
        sha256 = "d086deab3ca0b74751fcc1905d268697b0d471e747fb50eced94941f28b35fb8",
        build_file_content = """
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
swift_library(
    name = "swift-protobuf",
    module_name = "SwiftProtobuf",
    srcs = glob(["Sources/SwiftProtobuf/**/*.swift"]),
    visibility = ["//visibility:public"],
)""",
    )
    
    return module_ctx.extension_metadata(
        reproducible = True,
        root_module_direct_deps = [
            "apple_swift_protobuf",
        ],
        root_module_direct_dev_deps = [],
    )

non_module_deps = module_extension(
    implementation = _non_module_deps_impl,
)

