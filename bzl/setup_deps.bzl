load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@io_bazel_rules_scala//scala:toolchains.bzl", "scala_register_toolchains")
load("@io_bazel_rules_scala//scala:scala.bzl", "scala_repositories")
load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

def djinni_setup_deps():
    bazel_skylib_workspace()

    rules_proto_dependencies()
    rules_proto_toolchains()

    scala_register_toolchains()
    scala_repositories()

    maven_install(
        name = "maven_djinni",
        artifacts = [
            "com.google.code.findbugs:jsr305:3.0.2",
            "junit:junit:4.12",
            "org.scala-lang.modules:scala-parser-combinators_2.11:1.0.1",
            "org.yaml:snakeyaml:1.15",
            "com.github.scopt:scopt_2.11:3.2.0",
            "io.reactivex.rxjava2:rxjava:2.2.21"
        ],
        repositories = ["https://maven.google.com", "https://repo1.maven.org/maven2"],
    )
