"""Analysis tests for Djinni's source provider and language action contracts."""

load("@bazel_skylib//lib:unittest.bzl", "analysistest", "asserts")
load("//bzl:djinni_codegen.bzl", "DjinniInfo", "cc_djinni_library", "djinni_codegen", "djinni_language_codegen", "djinni_library", "java_djinni_library")

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
    return analysistest.end(env)

_codegen_test = analysistest.make(_codegen_test_impl, attrs = {
    "output_flag": attr.string(),
    "other_output_flag": attr.string(),
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
    java_djinni_library(
        name = "analysis_java",
        deps = [":shared-schema"],
        outs = outs,
        java_use_final_for_record = False,
        testonly = True,
        tags = ["manual"],
    )
    _provider_test(name = "provider_test", target_under_test = ":shared-schema")
    _codegen_test(
        name = "cpp_action_test",
        target_under_test = ":analysis_cc__djinni_cpp_codegen",
        output_flag = "--cpp-out",
        other_output_flag = "--java-out",
    )
    _codegen_test(
        name = "java_action_test",
        target_under_test = ":analysis_java__djinni_java_codegen",
        output_flag = "--java-out",
        other_output_flag = "--cpp-out",
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
        tests = [":provider_test", ":cpp_action_test", ":java_action_test", ":ambiguous_input_test", ":invalid_include_test", ":c_compatibility_test", ":objc_compatibility_test", ":yaml_compatibility_test"],
    )
