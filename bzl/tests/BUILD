load("//bzl:djinni_codegen.bzl", "c_djinni_library", "cc_djinni_library", "djinni_language_codegen", "djinni_library", "java_djinni_library", "jni_djinni_library", "objc_djinni_library", "ts_djinni_filegroup", "wasm_djinni_cc_library")
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
    cpp_namespace = "shared::defaults",
    cpp_include_prefix = "shared/",
    cpp_base_lib_include_prefix = "djinni/cpp/",
    cpp_optional_header = "<optional>",
    cpp_optional_template = "std::optional",
    idl = "idl/shared.djinni",
    idl_include_files = ["idl/nested.djinni"],
    idl_include_paths = ["bzl/tests/idl"],
    visibility = ["//visibility:public"],
)

cc_djinni_library(
    name = "tree_probe",
    deps = [":shared-schema"],
    testonly = True,
)

cc_test(
    name = "tree_probe_consumer",
    srcs = ["tree_probe_consumer.cc"],
    deps = [":tree_probe"],
    testonly = True,
)

java_djinni_library(
    name = "java_tree_probe",
    deps = [":shared-schema"],
    testonly = True,
)

djinni_library(
    name = "tree-multi-schema",
    idl = "idl/tree_multi.djinni",
    cpp_namespace = "tree_multi",
)

cc_djinni_library(
    name = "tree_multi_probe",
    deps = [":tree-multi-schema"],
)

djinni_library(
    name = "complex-schema",
    idl = "//test-suite:djinni/all.djinni",
    srcs = ["//test-suite:complex-schema-inputs"],
    idl_include_paths = ["test-suite/djinni/vendor"],
    verify = False,
)

cc_djinni_library(
    name = "complex_tree_probe",
    deps = [":complex-schema"],
    cpp_namespace = "testsuite",
    cc_deps = ["//test-suite:complex-handwritten-headers", "//support-lib:djinni-support-common"],
)

djinni_language_codegen(
    name = "c_tree_probe",
    deps = [":shared-schema"],
    language = "c",
    c_include_prefix = "shared/",
    testonly = True,
)

djinni_language_codegen(
    name = "jni_tree_probe",
    deps = [":shared-schema"],
    language = "jni",
    jni_include_prefix = "shared/",
    jni_include_cpp_prefix = "shared/",
    testonly = True,
)

objc_djinni_library(
    name = "objc_tree_probe",
    deps = [":shared-schema"],
    objc_type_prefix = "OBJ",
    objc_include_prefix = "shared/",
    objcpp_include_cpp_prefix = "shared/",
    objc_deps = [":tree_probe", "//support-lib:djinni-support-objc"],
    testonly = True,
    tags = ["manual"],
)

wasm_djinni_cc_library(
    name = "wasm_tree_probe",
    deps = [":shared-schema"],
    wasm_include_prefix = "shared/",
    cc_deps = [":tree_probe", "//support-lib:djinni-support-wasm"],
    testonly = True,
    tags = ["manual"],
)

ts_djinni_filegroup(
    name = "ts_tree_probe",
    deps = [":shared-schema"],
)

djinni_language_codegen(
    name = "low_level_tree_probe",
    deps = [":shared-schema"],
    language = "cpp",
)
