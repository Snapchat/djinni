load("@io_bazel_rules_scala//scala:scala.bzl", "scala_binary")

scala_binary(
    name = "djinni",
    srcs = glob(["source/**/*.scala"]),
    main_class = "djinni.Main",
    deps = [
        "@maven_djinni//:com_github_scopt_scopt_2_11",
        "@maven_djinni//:org_scala_lang_modules_scala_parser_combinators_2_11",
        "@maven_djinni//:org_yaml_snakeyaml",
    ],
    visibility = ["//visibility:public"],
)
