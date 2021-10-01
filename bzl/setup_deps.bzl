load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@io_bazel_rules_scala//scala:toolchains.bzl", "scala_register_toolchains")
load("@io_bazel_rules_scala//scala:scala.bzl", "scala_repositories")
load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@build_bazel_rules_apple//apple:repositories.bzl", "apple_rules_dependencies",)
load("@build_bazel_rules_swift//swift:repositories.bzl", "swift_rules_dependencies",)
load("@build_bazel_apple_support//lib:repositories.bzl", "apple_support_dependencies",)
load("@io_bazel_rules_kotlin//kotlin:kotlin.bzl", "kotlin_repositories", "kt_register_toolchains")

def djinni_setup_deps():

    bazel_skylib_workspace()

    scala_register_toolchains()
    scala_repositories()

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

    apple_rules_dependencies()
    swift_rules_dependencies()
    apple_support_dependencies()

    kotlin_repositories()
    kt_register_toolchains()
