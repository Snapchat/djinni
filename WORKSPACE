workspace(name="snap_djinni")

load("//bzl:deps.bzl", "djinni_deps")
djinni_deps()

load("//bzl:setup_deps.bzl", "djinni_setup_deps")
djinni_setup_deps()

# --- Everything below is only used for examples and tests

android_sdk_repository(name = "androidsdk")
android_ndk_repository(name = "androidndk", api_level = 21)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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
    url = "https://github.com/bazelbuild/rules_kotlin/archive/{}.zip".format(rules_kotlin_version),
    type = "zip",
    strip_prefix = "rules_kotlin-{}".format(rules_kotlin_version),
    sha256 = "4fd769fb0db5d3c6240df8a9500515775101964eebdf85a3f9f0511130885fde",
)

load("@build_bazel_rules_apple//apple:repositories.bzl", "apple_rules_dependencies")
load("@build_bazel_rules_swift//swift:repositories.bzl", "swift_rules_dependencies")
load("@build_bazel_apple_support//lib:repositories.bzl", "apple_support_dependencies")
load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kotlin_repositories", "kt_register_toolchains")

apple_rules_dependencies()
swift_rules_dependencies()
apple_support_dependencies()

kotlin_repositories()
kt_register_toolchains()

emsdk_version = "2.0.29"

http_archive(
    name = "emsdk",
    strip_prefix = "emsdk-%s/bazel" % emsdk_version,
    type = "zip",
    url = "https://github.com/emscripten-core/emsdk/archive/%s.zip" % emsdk_version,
    sha256 = "e0194dba792a978bd2dfd5ba3b5855e8ff391d48be76238bc9faec1e95ba1d31",
)

load("@emsdk//:deps.bzl", "deps")
deps()

load("@emsdk//:emscripten_deps.bzl", "emscripten_deps")

emscripten_deps(emscripten_version = "2.0.29")
