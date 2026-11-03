load("//bzl:djinni_codegen.bzl", "c_djinni_library", "cc_djinni_library", "djinni_library", "java_djinni_library", "jni_djinni_library", "objc_djinni_library", "ts_djinni_filegroup", "wasm_djinni_cc_library")
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
    cpp_base_lib_include_prefix = "djinni/cpp/",
    cpp_include_prefix = "shared/",
    cpp_namespace = "shared::defaults",
    cpp_optional_header = "<optional>",
    cpp_optional_template = "std::optional",
    idl = "idl/shared.djinni",
    idl_include_files = ["idl/nested.djinni"],
    idl_include_paths = ["bzl/tests/idl"],
    project_name = "shared",
    visibility = ["//visibility:public"],
)

cc_djinni_library(
    name = "tree_probe",
    testonly = True,
    deps = [":shared-schema"],
)

c_djinni_library(
    name = "c_tree_probe",
    testonly = True,
    deps = [":shared-schema"],
)

cc_test(
    name = "tree_probe_consumer",
    testonly = True,
    srcs = ["tree_probe_consumer.cc"],
    deps = [":tree_probe"],
)

java_djinni_library(
    name = "java_tree_probe",
    testonly = True,
    java_package = "example.shared",
    deps = [":shared-schema"],
)

jni_djinni_library(
    name = "jni_tree_probe",
    testonly = True,
    deps = [":shared-schema"],
)

java_test(
    name = "java_tree_probe_test",
    testonly = True,
    srcs = ["TreeProbe.java"],
    main_class = "example.shared.TreeProbe",
    use_testrunner = False,
    deps = [":java_tree_probe"],
)

java_djinni_library(
    name = "java_explicit_probe",
    testonly = True,
    outs = {"java_srcs": [
        "analysis/java_explicit/Shared.java",
        "analysis/java_explicit/Status.java",
    ]},
    java_package = "example.shared",
    deps = [":shared-schema"],
)

java_test(
    name = "java_explicit_probe_test",
    testonly = True,
    srcs = ["ExplicitTreeProbe.java"],
    main_class = "example.shared.ExplicitTreeProbe",
    use_testrunner = False,
    deps = [":java_explicit_probe"],
)

djinni_library(
    name = "tree-multi-schema",
    cpp_namespace = "tree_multi",
    idl = "idl/tree_multi.djinni",
)

cc_djinni_library(
    name = "tree_multi_probe",
    deps = [":tree-multi-schema"],
)

djinni_library(
    name = "complex-schema",
    srcs = ["//test-suite:complex-schema-inputs"],
    idl = "//test-suite:djinni/all.djinni",
    idl_include_paths = ["test-suite/djinni/vendor"],
    verify = False,
)

cc_djinni_library(
    name = "complex_tree_probe",
    cc_deps = [
        "//support-lib:djinni-support-common",
        "//test-suite:complex-handwritten-headers",
    ],
    cpp_namespace = "testsuite",
    deps = [":complex-schema"],
)

objc_djinni_library(
    name = "objc_inventory_probe",
    testonly = True,
    tags = ["manual"],
    deps = [":shared-schema"],
)

objc_djinni_library(
    name = "objc_tree_probe",
    testonly = True,
    objc_deps = [
        ":tree_probe",
        "//support-lib:djinni-support-objc",
    ],
    objc_include_prefix = "shared/",
    objc_type_prefix = "OBJ",
    objcpp_include_cpp_prefix = "shared/",
    tags = ["manual"],
    deps = [":shared-schema"],
)

wasm_djinni_cc_library(
    name = "wasm_inventory_probe",
    testonly = True,
    tags = ["manual"],
    deps = [":shared-schema"],
)

wasm_djinni_cc_library(
    name = "wasm_tree_probe",
    testonly = True,
    cc_deps = [
        ":tree_probe",
        "//support-lib:djinni-support-wasm",
    ],
    tags = ["manual"],
    wasm_include_prefix = "shared/",
    deps = [":shared-schema"],
)

ts_djinni_filegroup(
    name = "ts_tree_probe",
    deps = [":shared-schema"],
)

ts_djinni_filegroup(
    name = "ts_explicit_probe",
    outs = {"ts_srcs": ["analysis/ts_explicit/shared.ts"]},
    ts_module = "shared",
    deps = [":shared-schema"],
)

sh_test(
    name = "ts_outputs_test",
    srcs = ["ts_outputs_test.sh"],
    args = [
        "$(rootpath :ts_tree_probe)",
        "$(rootpath :ts_explicit_probe)",
    ],
    data = [
        ":ts_explicit_probe",
        ":ts_tree_probe",
    ],
)
