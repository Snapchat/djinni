load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def djinni_deps():
    # Bazel Skylib needed for Scala rules
    bazel_skylib_version = "1.0.2"
    maybe(
        name = "bazel_skylib",
        repo_rule = http_archive,
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
            "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
        ],
        sha256 = "97e70364e9249702246c0e9444bccdc4b847bed1eb03c5a3ece4f83dfe6abc44",
    )

    rules_scala_version = "2aba2cd5499f2f926c903cb5ef7c023b8cb28508"
    maybe(
        name = "io_bazel_rules_scala",
        repo_rule = http_archive,
        strip_prefix = "rules_scala-{}".format(rules_scala_version),
        type = "zip",
        url = "https://github.com/bazelbuild/rules_scala/archive/{}.zip".format(rules_scala_version),
        sha256 = "f0c7b96e3e778cec41125501782734ea41ecb42f3e00c05786818438a434b4c7",
    )

    protobuf_version = "3.12.4"
    maybe(
        name = "com_google_protobuf",
        repo_rule = http_archive,
        url = "https://github.com/protocolbuffers/protobuf/archive/v{}.tar.gz".format(protobuf_version),
        strip_prefix = "protobuf-{}".format(protobuf_version),
        sha256 = "512e5a674bf31f8b7928a64d8adf73ee67b8fe88339ad29adaa3b84dbaa570d8",
    )

    rules_jvm_external_tag = "3.0"
    maybe(
        name = "rules_jvm_external",
        repo_rule = http_archive,
        strip_prefix = "rules_jvm_external-{}".format(rules_jvm_external_tag),
        url = "https://github.com/bazelbuild/rules_jvm_external/archive/{}.zip".format(rules_jvm_external_tag),
        sha256 = "62133c125bf4109dfd9d2af64830208356ce4ef8b165a6ef15bbff7460b35c3a",
    )
