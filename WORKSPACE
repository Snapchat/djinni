workspace(name="snap_djinni")

android_sdk_repository(name = "androidsdk")
android_ndk_repository(name = "androidndk")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Bazel Skylib needed for Scala rules
http_archive(
    name="bazel_skylib",
    urls=[
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
    ],
    sha256="97e70364e9249702246c0e9444bccdc4b847bed1eb03c5a3ece4f83dfe6abc44",
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Scala stuff
rules_scala_version = "2aba2cd5499f2f926c903cb5ef7c023b8cb28508"

http_archive(
    name="io_bazel_rules_scala",
    strip_prefix="rules_scala-%s" % rules_scala_version,
    type="zip",
    url="https://github.com/bazelbuild/rules_scala/archive/%s.zip"
    % rules_scala_version,
    sha256="f0c7b96e3e778cec41125501782734ea41ecb42f3e00c05786818438a434b4c7",
)

load("@io_bazel_rules_scala//scala:toolchains.bzl", "scala_register_toolchains")
load("@io_bazel_rules_scala//scala:scala.bzl", "scala_repositories")

scala_register_toolchains()
scala_repositories()

protobuf_version = "3.12.4"
protobuf_version_sha256 = (
    "512e5a674bf31f8b7928a64d8adf73ee67b8fe88339ad29adaa3b84dbaa570d8"
)

http_archive(
    name="com_google_protobuf",
    url="https://github.com/protocolbuffers/protobuf/archive/v%s.tar.gz"
    % protobuf_version,
    strip_prefix="protobuf-%s" % protobuf_version,
    sha256=protobuf_version_sha256,
)

RULES_JVM_EXTERNAL_TAG = "3.0"
RULES_JVM_EXTERNAL_SHA = (
    "62133c125bf4109dfd9d2af64830208356ce4ef8b165a6ef15bbff7460b35c3a"
)

# Dependencies
http_archive(
    name="rules_jvm_external",
    strip_prefix="rules_jvm_external-%s" % RULES_JVM_EXTERNAL_TAG,
    sha256=RULES_JVM_EXTERNAL_SHA,
    url="https://github.com/bazelbuild/rules_jvm_external/archive/%s.zip"
    % RULES_JVM_EXTERNAL_TAG,
)

load("@rules_jvm_external//:defs.bzl", "maven_install")

maven_install(
    artifacts=[
        "com.google.code.findbugs:jsr305:3.0.2",
        "junit:junit:4.12",
        "org.scala-lang.modules:scala-parser-combinators_2.11:1.0.1",
        "org.yaml:snakeyaml:1.15",
        "com.github.scopt:scopt_2.11:3.2.0",
    ],
    repositories=["https://maven.google.com", "https://repo1.maven.org/maven2"],
)

git_repository(
    name="build_bazel_rules_apple",
    commit="73f12ab72777d6863693e33bc9128552f1dd5361",
    remote="https://github.com/bazelbuild/rules_apple.git",
)

load("@build_bazel_rules_apple//apple:repositories.bzl", "apple_rules_dependencies",)
apple_rules_dependencies()

load("@build_bazel_rules_swift//swift:repositories.bzl", "swift_rules_dependencies",)
swift_rules_dependencies()

load("@build_bazel_apple_support//lib:repositories.bzl", "apple_support_dependencies",)
apple_support_dependencies()
