"""Analysis tests for Djinni's source provider and language action contracts."""

load("@bazel_skylib//lib:unittest.bzl", "analysistest", "asserts")
load("//bzl:djinni_codegen.bzl", "DjinniInfo", "c_djinni_library", "cc_djinni_library", "djinni_codegen", "djinni_library", "java_djinni_library", "jni_djinni_library", "objc_djinni_library", "ts_djinni_filegroup", "wasm_djinni_cc_library")

def _provider_test_impl(ctx):
    env = analysistest.begin(ctx)
    target = analysistest.target_under_test(env)
    info = target[DjinniInfo]
    asserts.equals(env, "typed.djinni", info.idl.basename)
    asserts.equals(env, ["nested.djinni", "typed.djinni"], sorted([f.basename for f in info.transitive_srcs.to_list()]))
    asserts.equals(env, {
        "enums": ["status"],
        "records": ["shared"],
        "interfaces": ["first", "second"],
    }, info.types)
    asserts.equals(env, ["typed-schema.yaml"], sorted([f.basename for f in info.yaml_files.to_list()]))
    asserts.equals(env, "typed-schema.yaml", info.yaml_tree.basename)
    asserts.equals(env, ["bzl/tests/idl"], info.idl_include_paths.to_list())
    asserts.equals(env, "typed-schema.declarations.tsv", info.declarations_manifest.basename)
    asserts.equals(env, "typed-schema.outputs", info.output_manifest.basename)
    asserts.equals(env, "shared", info.project_name)
    actions = analysistest.target_actions(env)
    actions_by_mnemonic = {action.mnemonic: action for action in actions}
    asserts.equals(env, ["DjinniDeclarations", "DjinniOutputInventory"], sorted(actions_by_mnemonic.keys()))
    asserts.true(env, "--skip-generation" not in actions_by_mnemonic["DjinniDeclarations"].argv)
    asserts.true(env, "--yaml-out" in actions_by_mnemonic["DjinniDeclarations"].argv)
    declaration_args = actions_by_mnemonic["DjinniDeclarations"].argv
    asserts.true(env, "--verify-type-lists" in declaration_args)
    for flag, name in [("--expected-enum", "status"), ("--expected-record", "shared"), ("--expected-interface", "first")]:
        asserts.true(env, flag in declaration_args, "Missing type-list argument: " + flag)
        asserts.equals(env, name, declaration_args[declaration_args.index(flag) + 1])
    asserts.true(env, "--skip-generation" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, "--list-out-files" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, "--cpp-out" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, "--ts-out" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, "--swift-out" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, "--swiftxx-out" in actions_by_mnemonic["DjinniOutputInventory"].argv)
    asserts.true(env, any([f.basename == "typed-schema.djinni.inputs" for f in target[DefaultInfo].files.to_list()]))
    return analysistest.end(env)

_provider_test = analysistest.make(_provider_test_impl)

def _codegen_test_impl(ctx):
    env = analysistest.begin(ctx)
    actions = [action for action in analysistest.target_actions(env) if action.mnemonic == "DjinniCodegen"]
    asserts.equals(env, 1, len(actions))
    action = actions[0]
    asserts.equals(env, "DjinniCodegen", action.mnemonic)
    asserts.true(env, ctx.attr.output_flag in action.argv)
    asserts.false(env, ctx.attr.other_output_flag in action.argv)
    asserts.true(env, "--list-out-files" in action.argv)
    if ctx.attr.output_flag == "--swift-out":
        asserts.true(env, "--swiftxx-out" in action.argv)
    inputs = [f.basename for f in action.inputs.to_list()]
    for basename in ["shared.djinni", "nested.djinni", "shared-schema.djinni.inputs", "shared-schema.declarations.tsv", "shared-schema.yaml"]:
        asserts.true(env, basename in inputs, "Missing transitive action input: " + basename)
    if ctx.attr.output_flag == "--java-out":
        index = action.argv.index("--java-use-final-for-record")
        asserts.equals(env, "false", action.argv[index + 1])
    for flag, value in ctx.attr.expected_options.items():
        asserts.true(env, flag in action.argv, "Missing inherited option: " + flag)
        index = action.argv.index(flag)
        asserts.equals(env, value, action.argv[index + 1])
    return analysistest.end(env)

_codegen_test = analysistest.make(_codegen_test_impl, attrs = {
    "output_flag": attr.string(),
    "other_output_flag": attr.string(),
    "expected_options": attr.string_dict(default = {}),
})

def _inventory_consumer_test_impl(ctx):
    env = analysistest.begin(ctx)
    actions = analysistest.target_actions(env)
    actions_by_mnemonic = {action.mnemonic: action for action in actions}
    asserts.equals(env, ["DjinniCodegen"], sorted(actions_by_mnemonic.keys()))
    codegen = actions_by_mnemonic["DjinniCodegen"]
    codegen_inputs = [f.basename for f in codegen.inputs.to_list()]
    codegen_outputs = [f.basename for f in codegen.outputs.to_list()]
    asserts.true(env, ctx.attr.schema_manifest + ".declarations.tsv" in codegen_inputs)
    asserts.false(env, ctx.attr.schema_manifest + ".outputs" in codegen_inputs)
    asserts.equals(env, sorted([ctx.attr.target_name + "." + ctx.attr.language, ctx.attr.target_name + ".outputs_by_declaration.tsv", ctx.attr.target_name + ".outputs"]), sorted(codegen_outputs))
    asserts.true(env, "--list-out-files" in codegen.argv)
    return analysistest.end(env)

_inventory_consumer_test = analysistest.make(_inventory_consumer_test_impl, attrs = {
    "language": attr.string(),
    "schema_manifest": attr.string(),
    "target_name": attr.string(),
})

def _additional_compiler_inputs_test_impl(ctx):
    env = analysistest.begin(ctx)
    compile_actions = [action for action in analysistest.target_actions(env) if action.mnemonic == "CppCompile"]
    asserts.true(env, compile_actions, "Expected a CppCompile action")
    input_basenames = [
        input_file.basename
        for action in compile_actions
        for input_file in action.inputs.to_list()
    ]
    asserts.true(env, ctx.attr.input_basename in input_basenames, "Missing additional compiler input: " + ctx.attr.input_basename)
    return analysistest.end(env)

_additional_compiler_inputs_test = analysistest.make(_additional_compiler_inputs_test_impl, attrs = {
    "input_basename": attr.string(),
})

def _failure_test_impl(ctx):
    env = analysistest.begin(ctx)
    asserts.expect_failure(env, ctx.attr.message)
    return analysistest.end(env)

_failure_test = analysistest.make(_failure_test_impl, expect_failure = True, attrs = {"message": attr.string()})

def _files_test_impl(ctx):
    env = analysistest.begin(ctx)
    target = analysistest.target_under_test(env)
    files = sorted([file.basename for file in target[DefaultInfo].files.to_list()])
    asserts.equals(env, ctx.attr.expected, files)
    return analysistest.end(env)

_files_test = analysistest.make(_files_test_impl, attrs = {
    "expected": attr.string_list(),
})

def _source_outputs_test_impl(ctx):
    env = analysistest.begin(ctx)
    target = analysistest.target_under_test(env)
    files = sorted([file.basename for file in target[DefaultInfo].files.to_list()])
    actions = analysistest.target_actions(env)
    asserts.equals(env, ctx.attr.expected_files, files)
    asserts.equals(env, ctx.attr.expected_action_count, len(actions))
    asserts.equals(env, [ctx.attr.expected_mnemonic] * ctx.attr.expected_action_count, sorted([action.mnemonic for action in actions]))
    return analysistest.end(env)

_source_outputs_test = analysistest.make(_source_outputs_test_impl, attrs = {
    "expected_action_count": attr.int(),
    "expected_files": attr.string_list(),
    "expected_mnemonic": attr.string(),
})

def rule_tests(name):
    """Creates rule contract tests without executing the Scala compiler.

    Args:
        name: Test suite name.
    """
    cc_djinni_library(
        name = "analysis_cc",
        deps = [":shared-schema"],
        testonly = True,
        tags = ["manual"],
        library_kwargs = {"defines": ["ANALYSIS_TEST=1"]},
    )
    cc_djinni_library(
        name = "analysis_cc_override",
        deps = [":shared-schema"],
        cpp_namespace = "override::namespace",
        tags = ["manual"],
    )
    djinni_codegen(
        name = "invalid_project_name",
        djinni = ":shared-schema",
        project_name = "other",
        tree_languages = ["cpp"],
        tags = ["manual"],
    )
    _failure_test(
        name = "project_name_mismatch_test",
        target_under_test = ":invalid_project_name",
        message = "does not match djinni_library project_name",
    )
    djinni_library(
        name = "duplicate_type_list",
        idl = "idl/shared.djinni",
        types = {"records": ["shared", "shared"]},
        tags = ["manual"],
    )
    _failure_test(
        name = "duplicate_type_list_test",
        target_under_test = ":duplicate_type_list",
        message = "appears more than once",
    )
    djinni_library(
        name = "invalid_type_category",
        idl = "idl/shared.djinni",
        types = {"classes": ["shared"]},
        tags = ["manual"],
    )
    _failure_test(
        name = "invalid_type_category_test",
        target_under_test = ":invalid_type_category",
        message = "Unknown type category",
    )
    java_djinni_library(
        name = "analysis_java",
        deps = [":shared-schema"],
        ident_java_type = "foo_bar",
        java_use_final_for_record = False,
        testonly = True,
        tags = ["manual"],
    )
    c_djinni_library(
        name = "analysis_c_lib",
        deps = [":shared-schema"],
        c_namespace = "analysis_",
        cc_deps = ["//support-lib:djinni-support-common"],
        tags = ["manual"],
    )
    objc_djinni_library(
        name = "analysis_objc_lib",
        deps = [":shared-schema"],
        objc_deps = ["//support-lib:djinni-support-objc"],
        tags = ["manual"],
    )
    jni_djinni_library(
        name = "analysis_jni_lib",
        deps = [":shared-schema"],
        cc_deps = ["//support-lib:djinni-support-jni"],
        tags = ["manual"],
    )
    wasm_djinni_cc_library(
        name = "analysis_wasm_lib",
        deps = [":shared-schema"],
        cc_deps = ["//support-lib:djinni-support-wasm"],
        ident_wasm_class = "NativeFooBar",
        ident_wasm_file = "NativeFooBar",
        wasm_cpp_namespace = "wasm::bridge",
        tags = ["manual"],
    )
    _additional_compiler_inputs_test(
        name = "wasm_additional_compiler_inputs_test",
        target_under_test = "//bzl/tests:wasm_typed_probe",
        input_basename = "nested.djinni",
    )
    ts_djinni_filegroup(
        name = "analysis_ts_files",
        deps = [":shared-schema"],
        ts_module = "shared",
        tags = ["manual"],
    )
    _files_test(
        name = "ts_filegroup_test",
        target_under_test = ":analysis_ts_files",
        expected = ["analysis_ts_files.ts"],
    )
    _source_outputs_test(
        name = "typed_cpp_sources_test",
        target_under_test = ":typed_cpp_probe__generated_sources",
        expected_files = ["first__cpp.cpp", "second__cpp.cpp", "shared__cpp.cpp"],
        expected_mnemonic = "DjinniTypedSource",
        expected_action_count = 3,
    )
    _source_outputs_test(
        name = "tree_fallback_sources_test",
        target_under_test = ":c_tree_probe__generated_sources",
        expected_files = ["c_tree_probe__generated_sources__unity.cpp"],
        expected_mnemonic = "DjinniUnityFallback",
        expected_action_count = 1,
    )
    _provider_test(name = "provider_test", target_under_test = ":typed-schema")
    for language, target_name, schema_manifest in [
        ("cpp", "typed_cpp_probe__djinni_cpp_codegen", "typed-schema"),
        ("c", "c_tree_probe__djinni_c_codegen", "c-schema"),
        ("java", "java_probe__djinni_java_codegen", "shared-schema"),
        ("jni", "jni_typed_probe__djinni_jni_codegen", "shared-schema"),
        ("objc", "objc_typed_probe__djinni_objc_codegen", "shared-schema"),
        ("wasm", "wasm_typed_probe__djinni_wasm_codegen", "shared-schema"),
        ("ts", "ts_tree_probe__djinni_ts_codegen", "shared-schema"),
    ]:
        _inventory_consumer_test(
            name = language + "_inventory_consumer_test",
            target_under_test = "//bzl/tests:" + target_name,
            language = language,
            schema_manifest = schema_manifest,
            target_name = target_name,
        )
    _codegen_test(
        name = "cpp_action_test",
        target_under_test = ":analysis_cc__djinni_cpp_codegen",
        output_flag = "--cpp-out",
        other_output_flag = "--java-out",
        expected_options = {
            "--cpp-namespace": "shared::defaults",
            "--cpp-include-prefix": "shared/",
            "--cpp-base-lib-include-prefix": "djinni/cpp/",
            "--cpp-optional-template": "std::optional",
            "--cpp-optional-header": "<optional>",
        },
    )
    _codegen_test(
        name = "cpp_override_test",
        target_under_test = ":analysis_cc_override__djinni_cpp_codegen",
        output_flag = "--cpp-out",
        other_output_flag = "--java-out",
        expected_options = {
            "--cpp-namespace": "override::namespace",
            "--cpp-optional-template": "std::optional",
        },
    )
    _codegen_test(
        name = "java_action_test",
        target_under_test = ":analysis_java__djinni_java_codegen",
        output_flag = "--java-out",
        other_output_flag = "--cpp-out",
        expected_options = {"--ident-java-type": "foo_bar"},
    )
    _codegen_test(
        name = "swift_action_test",
        target_under_test = ":swift_codegen_probe",
        output_flag = "--swift-out",
        other_output_flag = "--objc-out",
        expected_options = {
            "--swift-module": "Shared",
            "--swiftxx-include-prefix": "shared/",
            "--ident-swiftxx-class": "SwiftShared",
        },
    )
    for language, target, flag in [
        ("c", ":analysis_c_lib__djinni_c_codegen", "--c-out"),
        ("objc", ":analysis_objc_lib__djinni_objc_codegen", "--objc-out"),
        ("jni", ":analysis_jni_lib__djinni_jni_codegen", "--jni-out"),
        ("wasm", ":analysis_wasm_lib__djinni_wasm_codegen", "--wasm-out"),
        ("ts", ":analysis_ts_files__djinni_ts_codegen", "--ts-out"),
    ]:
        _codegen_test(
            name = language + "_library_action_test",
            target_under_test = target,
            output_flag = flag,
            other_output_flag = "--java-out",
            expected_options = {"--cpp-namespace": "shared::defaults"},
        )
    _codegen_test(
        name = "wasm_naming_action_test",
        target_under_test = ":analysis_wasm_lib__djinni_wasm_codegen",
        output_flag = "--wasm-out",
        other_output_flag = "--jni-out",
        expected_options = {
            "--wasm-cpp-namespace": "wasm::bridge",
            "--ident-wasm-class": "NativeFooBar",
            "--ident-wasm-file": "NativeFooBar",
        },
    )
    djinni_codegen(
        name = "invalid_codegen",
        djinni = ":shared-schema",
        idl = "idl/shared.djinni",
        tree_languages = ["cpp"],
        tags = ["manual"],
    )
    _failure_test(
        name = "ambiguous_input_test",
        target_under_test = ":invalid_codegen",
        message = "Specify only one of djinni or idl",
    )
    djinni_library(
        name = "invalid_include",
        idl = "idl/shared.djinni",
        idl_include_paths = ["../outside"],
        tags = ["manual"],
    )
    _failure_test(
        name = "invalid_include_test",
        target_under_test = ":invalid_include",
        message = "idl_include_paths must be repository-relative",
    )
    native.test_suite(
        name = name,
        tests = [":provider_test", ":duplicate_type_list_test", ":invalid_type_category_test", ":typed_cpp_sources_test", ":tree_fallback_sources_test", ":cpp_inventory_consumer_test", ":c_inventory_consumer_test", ":java_inventory_consumer_test", ":jni_inventory_consumer_test", ":objc_inventory_consumer_test", ":wasm_inventory_consumer_test", ":ts_inventory_consumer_test", "//bzl/tests:typed_cpp_probe_consumer", "//bzl/tests:tree_fallback_consumer", "//bzl/tests:java_tree_probe_test", "//bzl/tests:ts_outputs_test", ":cpp_action_test", ":cpp_override_test", ":java_action_test", ":swift_action_test", ":c_library_action_test", ":objc_library_action_test", ":jni_library_action_test", ":wasm_library_action_test", ":wasm_naming_action_test", ":wasm_additional_compiler_inputs_test", ":ts_library_action_test", ":ts_filegroup_test", ":ambiguous_input_test", ":invalid_include_test", ":project_name_mismatch_test"],
    )
