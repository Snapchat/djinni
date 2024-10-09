workspace(name="snap_djinni")

load("//bzl:deps.bzl", "djinni_deps")
djinni_deps()
load("//bzl:scala_config.bzl", "djinni_scala_config")
djinni_scala_config()
load("//bzl:setup_deps.bzl", "djinni_setup_deps")
djinni_setup_deps()

# --- Everything below is only used for examples and tests

load("//bzl:android_configure.bzl", "android_configure")
android_configure(name = "local_config_android")
load("@local_config_android//:android_configure.bzl", "android_workspace")
android_workspace()

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

build_bazel_rules_apple_version = "1.1.3"
http_archive(
    name = "build_bazel_rules_apple",
    sha256 = "f94e6dddf74739ef5cb30f000e13a2a613f6ebfa5e63588305a71fce8a8a9911",
    url = "https://github.com/bazelbuild/rules_apple/releases/download/{0}/rules_apple.{0}.tar.gz" .format(build_bazel_rules_apple_version),
)

build_bazel_rules_swift_version = "1.14.0"
http_archive(
    name = "build_bazel_rules_swift",
    sha256 = "9b0064197e3b6c123cf7cbd377ad5071ac020cbd208fcc23dbc9f3928baf4fa2",
    url = "https://github.com/bazelbuild/rules_swift/releases/download/{0}/rules_swift.{0}.tar.gz" .format(build_bazel_rules_swift_version),
)

build_bazel_apple_support_version = "1.15.1"
http_archive(
    name = "build_bazel_apple_support",
    sha256 = "c4bb2b7367c484382300aee75be598b92f847896fb31bbd22f3a2346adf66a80",
    url = "https://github.com/bazelbuild/apple_support/releases/download/{0}/apple_support.{0}.tar.gz" .format(build_bazel_apple_support_version),
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
load("@build_bazel_apple_support//lib:repositories.bzl", "apple_support_dependencies")
load("@build_bazel_rules_swift//swift:repositories.bzl", "swift_rules_dependencies")
load("@build_bazel_rules_swift//swift:extras.bzl", "swift_rules_extra_dependencies",)
load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kotlin_repositories", "kt_register_toolchains")

apple_rules_dependencies()
apple_support_dependencies()
swift_rules_dependencies()
swift_rules_extra_dependencies()

kotlin_repositories()
kt_register_toolchains()

emsdk_version = "3.1.8"

http_archive(
    name = "emsdk",
    strip_prefix = "emsdk-%s/bazel" % emsdk_version,
    type = "zip",
    url = "https://github.com/emscripten-core/emsdk/archive/%s.zip" % emsdk_version,
    sha256 = "7795202a50ab09958d8943f79110de4386ff0f38bf4c97ec1a896885f28fe1cf",
)

load("@emsdk//:deps.bzl", emsdk_deps = "deps")
emsdk_deps()

load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")
emsdk_emscripten_deps()
