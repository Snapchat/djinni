load("@io_bazel_rules_scala//:scala_config.bzl", "scala_config")

def djinni_scala_config():
    scala_config(scala_version = "2.11.12")
