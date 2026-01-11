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
        sha256 = "5f1a3c09f045d6d9e435e930fe86be3ab96a79e19cd852f4e8f3761ce17cbf72",
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

