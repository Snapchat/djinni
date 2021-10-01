load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def djinni_deps():

    native.android_sdk_repository(name = "androidsdk")
    native.android_ndk_repository(name = "androidndk", api_level = 21)

    # Bazel Skylib needed for Scala rules
    bazel_skylib_version = "1.0.2"
    http_archive(
        name="bazel_skylib",
        urls=[
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
            "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz"
                .format(bazel_skylib_version),
        ],
        sha256="97e70364e9249702246c0e9444bccdc4b847bed1eb03c5a3ece4f83dfe6abc44",
    )

    rules_scala_version = "2aba2cd5499f2f926c903cb5ef7c023b8cb28508"
    http_archive(
        name="io_bazel_rules_scala",
        strip_prefix="rules_scala-{}".format(rules_scala_version),
        type="zip",
        url="https://github.com/bazelbuild/rules_scala/archive/{}.zip".format(rules_scala_version),
        sha256="f0c7b96e3e778cec41125501782734ea41ecb42f3e00c05786818438a434b4c7",
    )

    protobuf_version = "3.12.4"
    http_archive(
        name="com_google_protobuf",
        url="https://github.com/protocolbuffers/protobuf/archive/v{}.tar.gz".format(protobuf_version),
        strip_prefix="protobuf-{}".format(protobuf_version),
        sha256="512e5a674bf31f8b7928a64d8adf73ee67b8fe88339ad29adaa3b84dbaa570d8",
    )

    rules_jvm_external_tag = "3.0"
    http_archive(
        name="rules_jvm_external",
        strip_prefix="rules_jvm_external-{}".format(rules_jvm_external_tag),
        url="https://github.com/bazelbuild/rules_jvm_external/archive/{}.zip".format(rules_jvm_external_tag),
        sha256="62133c125bf4109dfd9d2af64830208356ce4ef8b165a6ef15bbff7460b35c3a",
    )

    build_bazel_rules_apple_version = "0.31.3"
    http_archive(
        name = "build_bazel_rules_apple",
        sha256 = "0052d452af7742c8f3a4e0929763388a66403de363775db7e90adecb2ba4944b",
        url = "https://github.com/bazelbuild/rules_apple/releases/download/{0}/rules_apple.{0}.tar.gz"
            .format(build_bazel_rules_apple_version),
    )

    build_bazel_rules_swift_version = "0.23.0"
    http_archive(
        name = "build_bazel_rules_swift",
        sha256 = "f872c0388808c3f8de67e0c6d39b0beac4a65d7e07eff3ced123d0b102046fb6",
        url = "https://github.com/bazelbuild/rules_swift/releases/download/{0}/rules_swift.{0}.tar.gz"
            .format(build_bazel_rules_swift_version),
    )

    build_bazel_apple_support_version = "0.11.0"
    http_archive(
        name = "build_bazel_apple_support",
        sha256 = "76df040ade90836ff5543888d64616e7ba6c3a7b33b916aa3a4b68f342d1b447",
        url = "https://github.com/bazelbuild/apple_support/releases/download/{0}/apple_support.{0}.tar.gz"
            .format(build_bazel_apple_support_version),
    )

    rules_kotlin_version = "legacy-1.3.0"
    http_archive(
        name = "io_bazel_rules_kotlin",
        urls = ["https://github.com/bazelbuild/rules_kotlin/archive/{}.zip".format(rules_kotlin_version)],
        type = "zip",
        strip_prefix = "rules_kotlin-{}".format(rules_kotlin_version),
        sha256 = "4fd769fb0db5d3c6240df8a9500515775101964eebdf85a3f9f0511130885fde",
    )

