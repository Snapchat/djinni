workspace(name = "snap_djinni")

load("@bazel_tools//tools/build_defs/repo:jvm.bzl", "jvm_maven_import_external")

jvm_maven_import_external(
    name = "jsr305",
    artifact = "com.google.code.findbugs:jsr305:3.0.2",
    artifact_sha256 = "766ad2a0783f2687962c8ad74ceecc38a28b9f72a2d085ee438b7813e928d0c7",
    licenses = ["notice"],  # Apache 2.0
    server_urls = ["https://repo.maven.apache.org/maven2/"],
)

jvm_maven_import_external(
    name = "junit",
    artifact = "junit:junit:4.12",
    artifact_sha256 = "59721f0805e223d84b90677887d9ff567dc534d7c502ca903c0c2b17f05c116a",
    licenses = ["notice"],  # EPL 1.0
    server_urls = ["https://repo.maven.apache.org/maven2/"],
    deps = ["@hamcrest_core"],
)

jvm_maven_import_external(
    name = "hamcrest_core",
    artifact = "org.hamcrest:hamcrest-core:jar:1.3",
    artifact_sha256 = "66fdef91e9739348df7a096aa384a5685f4e875584cce89386a7a47251c4d8e9",
    licenses = ["notice"],  # BSD
    server_urls = ["https://repo.maven.apache.org/maven2/"],
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "build_bazel_rules_apple",
    commit = "5131f3d46794bf227d296c82f30c2499c9de3c5b",
    remote = "https://github.com/bazelbuild/rules_apple.git",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()
