"""Non-module dependencies for Djinni"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def djinni_non_module_deps():
    """Loads dependencies that are not yet available in the Bazel Central Registry"""
    
    # Scala rules - not yet in BCR with full support
    rules_scala_version = "6.6.1"
    maybe(
        name = "io_bazel_rules_scala",
        repo_rule = http_archive,
        strip_prefix = "rules_scala-{}".format(rules_scala_version),
        url = "https://github.com/bazelbuild/rules_scala/releases/download/v{0}/rules_scala-v{0}.tar.gz".format(rules_scala_version),
        sha256 = "5d8824ba613c8c0a8c5078f1a5e07ea2b90fc8c0fe7a15a8e6b8e8e8e8e8e8e8",  # Update with actual SHA
    )
    
    # Kotlin rules for Android examples
    rules_kotlin_version = "2.0.1"
    maybe(
        name = "io_bazel_rules_kotlin",
        repo_rule = http_archive,
        url = "https://github.com/bazelbuild/rules_kotlin/releases/download/v{0}/rules_kotlin-v{0}.tar.gz".format(rules_kotlin_version),
        sha256 = "",  # Update with actual SHA if needed
    )
    
    # Emscripten for WebAssembly support
    emsdk_version = "3.1.68"
    maybe(
        name = "emsdk",
        repo_rule = http_archive,
        strip_prefix = "emsdk-%s/bazel" % emsdk_version,
        url = "https://github.com/emscripten-core/emsdk/archive/%s.tar.gz" % emsdk_version,
        sha256 = "",  # Update with actual SHA if needed
    )
    
    # Swift Protobuf
    swiftprotobuf_version = "1.28.2"
    maybe(
        name = "apple_swift_protobuf",
        repo_rule = http_archive,
        url = "https://github.com/apple/swift-protobuf/archive/refs/tags/{}.tar.gz".format(swiftprotobuf_version),
        strip_prefix = "swift-protobuf-{}".format(swiftprotobuf_version),
        sha256 = "",  # Update with actual SHA if needed
        build_file_content = """
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
swift_library(
    name = "swift-protobuf",
    module_name = "SwiftProtobuf",
    srcs = glob(["Sources/SwiftProtobuf/**/*.swift"]),
    visibility = ["//visibility:public"],
)""",
    )

def djinni_non_module_setup():
    """Setup function for dependencies loaded via djinni_non_module_deps"""
    
    # Emscripten setup
    # Note: These need to be called after emsdk is loaded
    # load("@emsdk//:deps.bzl", emsdk_deps = "deps")
    # emsdk_deps()
    # load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")
    # emsdk_emscripten_deps()
    pass

