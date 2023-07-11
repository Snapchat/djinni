load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def djinni_deps():
    # Bazel Skylib needed for Scala rules
    bazel_skylib_version = "1.0.3"
    maybe(
        name = "bazel_skylib",
        repo_rule = http_archive,
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
            "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
        ],
        sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    )
    rules_scala_version = "5.0.0"
    maybe(
        name = "io_bazel_rules_scala",
        repo_rule = http_archive,
        strip_prefix = "rules_scala-{}".format(rules_scala_version),
        url = "https://github.com/bazelbuild/rules_scala/archive/refs/tags/v{}.tar.gz".format(rules_scala_version),
        sha256 = "141a3919b37c80a846796f792dcf6ea7cd6e7b7ca4297603ca961cd22750c951",
    )    
    protobuf_version = "3.12.4"
    maybe(
        name = "com_google_protobuf",
        repo_rule = http_archive,
        url = "https://github.com/protocolbuffers/protobuf/archive/v{}.tar.gz".format(protobuf_version),
        strip_prefix = "protobuf-{}".format(protobuf_version),
        sha256 = "512e5a674bf31f8b7928a64d8adf73ee67b8fe88339ad29adaa3b84dbaa570d8",
    )
    rules_proto_version = "5.3.0-21.7"
    maybe(
        name = "rules_proto",
        repo_rule = http_archive,
        sha256 = "dc3fb206a2cb3441b485eb1e423165b231235a1ea9b031b4433cf7bc1fa460dd",
        strip_prefix = "rules_proto-{}".format(rules_proto_version),
        url = "https://github.com/bazelbuild/rules_proto/archive/refs/tags/{}.tar.gz".format(rules_proto_version)
    )
    rules_jvm_external_tag = "3.0"
    maybe(
        name = "rules_jvm_external",
        repo_rule = http_archive,
        strip_prefix = "rules_jvm_external-{}".format(rules_jvm_external_tag),
        url = "https://github.com/bazelbuild/rules_jvm_external/archive/{}.zip".format(rules_jvm_external_tag),
        sha256 = "62133c125bf4109dfd9d2af64830208356ce4ef8b165a6ef15bbff7460b35c3a",
    )
