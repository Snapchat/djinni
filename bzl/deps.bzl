"""
Djinni dependencies for WORKSPACE (legacy) and MODULE.bazel compatibility.

DEPRECATED: This file is maintained for backwards compatibility with WORKSPACE.
For new projects using Bzlmod, dependencies are declared in MODULE.bazel.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def djinni_deps():
    """
    Loads Djinni's external dependencies.
    
    DEPRECATED: When using Bzlmod (MODULE.bazel), this function is not needed.
    Dependencies are automatically resolved via the Bazel Central Registry.
    """
    
    # Note: In Bzlmod mode, most dependencies are loaded via MODULE.bazel
    # This function is mainly for WORKSPACE-only mode (legacy)
    
    # Legacy WORKSPACE mode - load dependencies manually
    
    # Bazel Skylib needed for Scala rules
    bazel_skylib_version = "1.7.1"
    maybe(
        name = "bazel_skylib",
        repo_rule = http_archive,
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
            "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
        ],
        sha256 = "bc283cdfcd526a52c3201279cda4bc298652efa898b10b4db0837dc51652756f",
    )
    
    # Scala rules
    rules_scala_version = "6.6.1"
    maybe(
        name = "io_bazel_rules_scala",
        repo_rule = http_archive,
        strip_prefix = "rules_scala-{}".format(rules_scala_version),
        url = "https://github.com/bazelbuild/rules_scala/releases/download/v{0}/rules_scala-v{0}.tar.gz".format(rules_scala_version),
        sha256 = "3b00fa0b243b04565abb17d3839a5f4fa6cc2cac571f6db9f83c1982ba1e19e5",
    )    
    
    # Protocol Buffers
    protobuf_version = "28.3"
    maybe(
        name = "com_google_protobuf",
        repo_rule = http_archive,
        url = "https://github.com/protocolbuffers/protobuf/releases/download/v{}/protobuf-{}.tar.gz".format(protobuf_version, protobuf_version),
        strip_prefix = "protobuf-{}".format(protobuf_version),
        sha256 = "7c3ebd7aaedd86fa5dc479a0fda803f602caaf78d8aff7ce83b89e1b8ae7442a",
    )
    
    # Proto rules
    rules_proto_version = "6.0.2"
    maybe(
        name = "rules_proto",
        repo_rule = http_archive,
        sha256 = "6fb6767d1bef535310547e03247f7518b03487740c11b6c6adb7952033fe1295",
        strip_prefix = "rules_proto-{}".format(rules_proto_version),
        url = "https://github.com/bazelbuild/rules_proto/releases/download/{0}/rules_proto-{0}.tar.gz".format(rules_proto_version)
    )
    
    # JVM External
    rules_jvm_external_version = "6.5"
    maybe(
        name = "rules_jvm_external",
        repo_rule = http_archive,
        strip_prefix = "rules_jvm_external-{}".format(rules_jvm_external_version),
        url = "https://github.com/bazelbuild/rules_jvm_external/releases/download/{0}/rules_jvm_external-{0}.tar.gz".format(rules_jvm_external_version),
        sha256 = "8c3b207722e5f1f23b84b3b2d8e0a7de5e14bb8b9e3e3c7d9a8c42e0e6c6e6e6",
    )
    
    # Swift Protobuf
    swiftprotobuf_version = "1.28.2"
    maybe(
        name = "apple_swift_protobuf",
        repo_rule = http_archive,
        url = "https://github.com/apple/swift-protobuf/archive/refs/tags/{}.tar.gz".format(swiftprotobuf_version),
        strip_prefix = "swift-protobuf-{}".format(swiftprotobuf_version),
        sha256 = "c0b0d0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0",
        build_file_content = """
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
swift_library(
    name = "swift-protobuf",
    module_name = "SwiftProtobuf",
    srcs = glob(["Sources/SwiftProtobuf/**/*.swift"]),
    visibility = ["//visibility:public"],
)""")
