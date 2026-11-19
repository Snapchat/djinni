load("//bzl:djinni_codegen.bzl", "c_djinni_library", "cc_djinni_library", "djinni_codegen", "djinni_library", "java_djinni_library", "jni_djinni_library", "objc_djinni_library", "ts_djinni_filegroup", "wasm_djinni_cc_library")
load(":rules_test.bzl", "rule_tests")

rule_tests(name = "rule_tests")

exports_files([
    "idl/shared.djinni",
    "idl/nested.djinni",
    "idl/typed.djinni",
])

# An external consumer imports these through a repository-relative search path.
djinni_library(
    name = "shared-schema",
    types = {
        "enums": ["status"],
        "records": ["shared"],
    },
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

djinni_library(
    name = "typed-schema",
    types = {
        "enums": ["status"],
        "records": ["shared"],
        "interfaces": ["first", "second"],
    },
    srcs = ["idl/nested.djinni"],
    cpp_base_lib_include_prefix = "djinni/cpp/",
    cpp_include_prefix = "shared/",
    cpp_namespace = "shared::defaults",
    cpp_optional_header = "<optional>",
    cpp_optional_template = "std::optional",
    idl = "idl/typed.djinni",
    idl_include_files = ["idl/nested.djinni"],
    idl_include_paths = ["bzl/tests/idl"],
    project_name = "shared",
)

cc_djinni_library(
    name = "typed_cpp_probe",
    testonly = True,
    deps = [":typed-schema"],
)

c_djinni_library(
    name = "c_tree_probe",
    testonly = True,
    c_namespace = "c_",
    cc_deps = [
        ":typed_cpp_probe",
        "//support-lib:djinni-support-common",
    ],
    tags = ["manual"],
    deps = [":c-schema"],
)

djinni_library(
    name = "c-schema",
    testonly = True,
    cpp_base_lib_include_prefix = "",
    cpp_include_prefix = "shared/",
    cpp_namespace = "shared::defaults",
    cpp_optional_header = "<optional>",
    cpp_optional_template = "std::optional",
    srcs = ["idl/nested.djinni"],
    idl = "idl/shared.djinni",
    idl_include_files = ["idl/nested.djinni"],
    idl_include_paths = ["bzl/tests/idl"],
)

cc_test(
    name = "typed_cpp_probe_consumer",
    testonly = True,
    srcs = ["tree_probe_consumer.cc"],
    deps = [":typed_cpp_probe"],
)

cc_test(
    name = "tree_fallback_consumer",
    testonly = True,
    srcs = ["tree_probe_consumer.cc"],
    deps = [":c_tree_probe"],
)

java_djinni_library(
    name = "java_probe",
    testonly = True,
    java_package = "example.shared",
    deps = [":shared-schema"],
)

jni_djinni_library(
    name = "jni_typed_probe",
    testonly = True,
    cc_deps = [":typed_cpp_probe", "//support-lib:djinni-support-jni"],
    jni_include_cpp_prefix = "shared/",
    tags = ["manual"],
    deps = [":shared-schema"],
)

java_test(
    name = "java_tree_probe_test",
    testonly = True,
    srcs = ["TreeProbe.java"],
    main_class = "example.shared.TreeProbe",
    use_testrunner = False,
    deps = [":java_probe"],
)

djinni_codegen(
    name = "swift_codegen_probe",
    djinni = ":shared-schema",
    tree_languages = ["swift", "swiftxx"],
    swift_module = "Shared",
    swiftxx_include_prefix = "shared/",
    ident_swiftxx_class = "SwiftShared",
    tags = ["manual"],
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
    name = "objc_typed_probe",
    testonly = True,
    objc_deps = [
        ":typed_cpp_probe",
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
    name = "wasm_typed_probe",
    testonly = True,
    additional_compiler_inputs = ["idl/nested.djinni"],
    cc_deps = [
        ":typed_cpp_probe",
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

sh_test(
    name = "ts_outputs_test",
    srcs = ["ts_outputs_test.sh"],
    args = ["$(rootpath :ts_tree_probe)"],
    data = [
        ":ts_tree_probe",
    ],
)
