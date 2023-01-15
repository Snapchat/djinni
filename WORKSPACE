workspace(name="snap_djinni")

load("//bzl:deps.bzl", "djinni_deps")
djinni_deps()
load("//bzl:scala_config.bzl", "djinni_scala_config")
djinni_scala_config()
load("//bzl:setup_deps.bzl", "djinni_setup_deps")
djinni_setup_deps()

# --- Everything below is only used for examples and tests

# android_sdk_repository fails to find build_tools if we don't explicitly set a version.
android_sdk_repository(name = "androidsdk", build_tools_version = "32.0.0")
RULES_ANDROID_NDK_COMMIT= "81ec8b79dc50ee97e336a25724fdbb28e33b8d41"
RULES_ANDROID_NDK_SHA = "b29409496439cdcdb50a8e161c4953ca78a548e16d3ee729a1b5cd719ffdacbf"

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
    name = "rules_android_ndk",
    url = "https://github.com/bazelbuild/rules_android_ndk/archive/%s.zip" % RULES_ANDROID_NDK_COMMIT,
    sha256 = RULES_ANDROID_NDK_SHA,
    strip_prefix = "rules_android_ndk-%s" % RULES_ANDROID_NDK_COMMIT,
)
load("@rules_android_ndk//:rules.bzl", "android_ndk_repository")
android_ndk_repository(name = "androidndk")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

build_bazel_rules_apple_version = "1.0.1"
http_archive(
    name = "build_bazel_rules_apple",
    sha256 = "36072d4f3614d309d6a703da0dfe48684ec4c65a89611aeb9590b45af7a3e592",
    url = "https://github.com/bazelbuild/rules_apple/releases/download/{0}/rules_apple.{0}.tar.gz"
        .format(build_bazel_rules_apple_version),
)

build_bazel_rules_swift_version = "1.0.0"
http_archive(
    name = "build_bazel_rules_swift",
    sha256 = "12057b7aa904467284eee640de5e33853e51d8e31aae50b3fb25d2823d51c6b8",
    url = "https://github.com/bazelbuild/rules_swift/releases/download/{0}/rules_swift.{0}.tar.gz"
        .format(build_bazel_rules_swift_version),
)

build_bazel_apple_support_version = "1.0.0"
http_archive(
    name = "build_bazel_apple_support",
    sha256 = "df317473b5894dd8eb432240d209271ebc83c76bb30c55481374b36ddf1e4fd1",
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
