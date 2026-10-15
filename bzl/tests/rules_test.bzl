"""Analysis tests for Djinni's source provider and language action contracts."""

load("@bazel_skylib//lib:unittest.bzl", "analysistest", "asserts")
load("//bzl:djinni_codegen.bzl", "DjinniInfo", "c_djinni_library", "cc_djinni_library", "djinni_codegen", "djinni_language_codegen", "djinni_library", "java_djinni_library", "jni_djinni_library", "objc_djinni_library", "ts_djinni_filegroup", "wasm_djinni_cc_library", "yaml_djinni_filegroup")

def _provider_test_impl(ctx):
    env = analysistest.begin(ctx)
    target = analysistest.target_under_test(env)
    info = target[DjinniInfo]
    asserts.equals(env, "shared.djinni", info.idl.basename)
    asserts.equals(env, ["nested.djinni", "shared.djinni"], sorted([f.basename for f in info.transitive_srcs.to_list()]))
    asserts.equals(env, ["bzl/tests/idl"], info.idl_include_paths.to_list())
    actions = analysistest.target_actions(env)
    asserts.equals(env, 1, len(actions))
    asserts.equals(env, "DjinniVerify", actions[0].mnemonic)
    asserts.true(env, "--skip-generation" in actions[0].argv)
    asserts.equals(env, 1, len(info.validation_files.to_list()))
    return analysistest.end(env)

_provider_test = analysistest.make(_provider_test_impl)

def _codegen_test_impl(ctx):
    env = analysistest.begin(ctx)
    actions = analysistest.target_actions(env)
    asserts.equals(env, 1, len(actions))
    action = actions[0]
    asserts.equals(env, "DjinniCodegen", action.mnemonic)
    asserts.true(env, ctx.attr.output_flag in action.argv)
    asserts.false(env, ctx.attr.other_output_flag in action.argv)
    inputs = [f.basename for f in action.inputs.to_list()]
    for basename in ["shared.djinni", "nested.djinni", "shared-schema.djinni.inputs"]:
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

def _failure_test_impl(ctx):
    env = analysistest.begin(ctx)
    asserts.expect_failure(env, ctx.attr.message)
    return analysistest.end(env)

_failure_test = analysistest.make(_failure_test_impl, expect_failure = True, attrs = {"message": attr.string()})

def _compatibility_test_impl(ctx):
    env = analysistest.begin(ctx)
    actions = analysistest.target_actions(env)
    asserts.equals(env, 1, len(actions))
    action = actions[0]
    for flag in ctx.attr.flags:
        asserts.true(env, flag in action.argv, "Missing option: " + flag)
    asserts.false(env, "--java-out" in action.argv)
    asserts.equals(env, ctx.attr.tree_count, len([f for f in action.outputs.to_list() if f.is_directory]))
    return analysistest.end(env)

_compatibility_test = analysistest.make(_compatibility_test_impl, attrs = {
    "flags": attr.string_list(),
    "tree_count": attr.int(),
})

def _files_test_impl(ctx):
    env = analysistest.begin(ctx)
    target = analysistest.target_under_test(env)
    files = sorted([file.basename for file in target[DefaultInfo].files.to_list()])
    asserts.equals(env, ctx.attr.expected, files)
    return analysistest.end(env)

_files_test = analysistest.make(_files_test_impl, attrs = {
    "expected": attr.string_list(),
})

def rule_tests(name):
    """Creates rule contract tests without executing the Scala compiler.

    Args:
        name: Test suite name.
    """
    outs = {
        "cpp_hdrs": ["analysis/cpp/shared.hpp", "analysis/cpp/status.hpp"],
        "java_srcs": ["analysis/java/Shared.java", "analysis/java/Status.java"],
    }
    cc_djinni_library(
        name = "analysis_cc",
        deps = [":shared-schema"],
        outs = outs,
        testonly = True,
        tags = ["manual"],
        library_kwargs = {"defines": ["ANALYSIS_TEST=1"]},
    )
    cc_djinni_library(
        name = "analysis_cc_override",
        deps = [":shared-schema"],
        outs = {"cpp_hdrs": ["analysis/cpp_override/shared.hpp"]},
        cpp_namespace = "override::namespace",
        cpp_legacy_records = False,
        tags = ["manual"],
    )
    java_djinni_library(
        name = "analysis_java",
        deps = [":shared-schema"],
        outs = outs,
        java_use_final_for_record = False,
        testonly = True,
        tags = ["manual"],
    )
    c_djinni_library(
        name = "analysis_c_lib",
        deps = [":shared-schema"],
        outs = {"c_srcs": ["analysis/c_library/shared.cpp"], "c_hdrs": ["analysis/c_library/shared.h"]},
        c_namespace = "analysis_",
        cc_deps = ["//support-lib:djinni-support-common"],
        tags = ["manual"],
    )
    objc_djinni_library(
        name = "analysis_objc_lib",
        deps = [":shared-schema"],
        outs = {"objc_hdrs": ["analysis/objc_library/Shared.h"], "objcpp_srcs": ["analysis/objc_library/Shared+Private.mm"]},
        objc_deps = ["//support-lib:djinni-support-objc"],
        tags = ["manual"],
    )
    jni_djinni_library(
        name = "analysis_jni_lib",
        deps = [":shared-schema"],
        outs = {"jni_srcs": ["analysis/jni_library/NativeShared.cpp"], "jni_hdrs": ["analysis/jni_library/NativeShared.hpp"]},
        cc_deps = ["//support-lib:djinni-support-jni"],
        tags = ["manual"],
    )
    wasm_djinni_cc_library(
        name = "analysis_wasm_lib",
        deps = [":shared-schema"],
        outs = {"wasm_srcs": ["analysis/wasm_library/NativeShared.cpp"], "wasm_hdrs": ["analysis/wasm_library/NativeShared.hpp"]},
        cc_deps = ["//support-lib:djinni-support-wasm"],
        tags = ["manual"],
    )
    ts_djinni_filegroup(
        name = "analysis_ts_files",
        deps = [":shared-schema"],
        outs = {"ts_srcs": ["analysis/ts_library/shared.ts"]},
        ts_module = "shared",
        tags = ["manual"],
    )
    yaml_djinni_filegroup(
        name = "analysis_yaml_files",
        deps = [":shared-schema"],
        outs = {"yaml_srcs": ["analysis/yaml_library/shared.yaml"]},
        yaml_metadata_languages = ["c", "wasm"],
        tags = ["manual"],
    )
    _files_test(
        name = "ts_filegroup_test",
        target_under_test = ":analysis_ts_files",
        expected = ["shared.ts"],
    )
    _files_test(
        name = "yaml_filegroup_test",
        target_under_test = ":analysis_yaml_files",
        expected = ["shared.yaml"],
    )
    _provider_test(name = "provider_test", target_under_test = ":shared-schema")
    _codegen_test(
        name = "cpp_action_test",
        target_under_test = ":analysis_cc__djinni_cpp_codegen",
        output_flag = "--cpp-out",
        other_output_flag = "--java-out",
        expected_options = {
            "--cpp-namespace": "shared::defaults",
            "--cpp-optional-template": "std::optional",
            "--cpp-optional-header": "<optional>",
            "--cpp-legacy-records": "true",
            "--ident-cpp-enum-type": "foo_bar",
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
            "--cpp-legacy-records": "false",
        },
    )
    _codegen_test(
        name = "java_action_test",
        target_under_test = ":analysis_java__djinni_java_codegen",
        output_flag = "--java-out",
        other_output_flag = "--cpp-out",
    )
    for language, target, flag in [
        ("c", ":analysis_c_lib__djinni_c_codegen", "--c-out"),
        ("objc", ":analysis_objc_lib__djinni_objc_codegen", "--objc-out"),
        ("jni", ":analysis_jni_lib__djinni_jni_codegen", "--jni-out"),
        ("wasm", ":analysis_wasm_lib__djinni_wasm_codegen", "--wasm-out"),
        ("ts", ":analysis_ts_files__djinni_ts_codegen", "--ts-out"),
        ("yaml", ":analysis_yaml_files__djinni_yaml_codegen", "--yaml-out"),
    ]:
        _codegen_test(
            name = language + "_library_action_test",
            target_under_test = target,
            output_flag = flag,
            other_output_flag = "--java-out",
        )
    djinni_codegen(
        name = "invalid_codegen",
        djinni = ":shared-schema",
        idl = "idl/shared.djinni",
        outs = {"cpp_hdrs": ["invalid/shared.hpp"]},
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
    for language, outputs, options, flags, trees in [
        ("c", {"c_srcs": ["analysis/c/shared.cpp"], "c_hdrs": ["analysis/c/include/shared.h"]}, {"c_namespace": "snap_", "cpp_legacy_records": True}, ["--c-out", "--c-header-out", "--cpp-legacy-records", "true"], 0),
        ("objc", {"objc_hdrs": ["analysis/objc/Shared.h"], "objcpp_srcs": ["analysis/objc/Shared+Private.mm"]}, {"objcpp_function_prologue_file": "utils/DjinniPrologue.hpp"}, ["--objc-out", "--objcpp-out", "utils/DjinniPrologue.hpp"], 0),
        ("yaml", {"yaml_srcs": ["analysis/yaml/shared.yaml"]}, {"yaml_metadata_languages": ["c", "wasm"]}, ["--yaml-out", "--c-out", "--wasm-out"], 2),
    ]:
        djinni_language_codegen(
            name = "compatibility_" + language,
            deps = [":shared-schema"],
            outs = dict(outputs, java_srcs = ["unused/Shared.java"]),
            language = language,
            tags = ["manual"],
            **options
        )
        _compatibility_test(
            name = language + "_compatibility_test",
            target_under_test = ":compatibility_" + language,
            flags = flags,
            tree_count = trees,
        )
    native.test_suite(
        name = name,
        tests = [":provider_test", ":cpp_action_test", ":cpp_override_test", ":java_action_test", ":c_library_action_test", ":objc_library_action_test", ":jni_library_action_test", ":wasm_library_action_test", ":ts_library_action_test", ":yaml_library_action_test", ":ts_filegroup_test", ":yaml_filegroup_test", ":ambiguous_input_test", ":invalid_include_test", ":c_compatibility_test", ":objc_compatibility_test", ":yaml_compatibility_test"],
    )
