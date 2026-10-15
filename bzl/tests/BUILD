load("//bzl:djinni_codegen.bzl", "djinni_library")
load(":rules_test.bzl", "rule_tests")

rule_tests(name = "rule_tests")

exports_files([
    "idl/shared.djinni",
    "idl/nested.djinni",
])

# An external consumer imports these through a repository-relative search path.
djinni_library(
    name = "shared-schema",
    srcs = ["idl/nested.djinni"],
    cpp_legacy_records = True,
    cpp_namespace = "shared::defaults",
    cpp_optional_header = "<optional>",
    cpp_optional_template = "std::optional",
    ident_cpp_enum_type = "foo_bar",
    idl = "idl/shared.djinni",
    idl_include_files = ["idl/nested.djinni"],
    idl_include_paths = ["bzl/tests/idl"],
    visibility = ["//visibility:public"],
)
