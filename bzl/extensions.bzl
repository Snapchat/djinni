"""Bazel module extensions for Djinni"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//bzl:android_configure.bzl", "android_configure")

# Tag classes for configuring the extension
_scala_toolchain_config_tag = tag_class(
    attrs = {
        "scala_version": attr.string(default = "2.11.12"),
    },
)

def _non_module_deps_impl(module_ctx):
    """Module extension to load non-BCR dependencies"""
    
    # Swift Protobuf
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
    
    # Android configuration
    android_configure(name = "local_config_android")
    
    return module_ctx.extension_metadata(
        reproducible = True,
        root_module_direct_deps = [
            "apple_swift_protobuf",
        ],
        root_module_direct_dev_deps = [],
    )

non_module_deps = module_extension(
    implementation = _non_module_deps_impl,
    tag_classes = {
        "scala_toolchain": _scala_toolchain_config_tag,
    },
)

# Toolchain registration extension
def _toolchain_setup_impl(module_ctx):
    """Module extension to set up toolchains after dependencies are loaded"""
    # This extension can be used to register toolchains that need to be
    # set up after repositories are loaded
    return module_ctx.extension_metadata(
        reproducible = True,
        root_module_direct_deps = [],
        root_module_direct_dev_deps = [],
    )

toolchain_setup = module_extension(
    implementation = _toolchain_setup_impl,
)

