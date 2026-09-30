"""Analysis tests for Djinni's source provider and language action contracts."""

load("@bazel_skylib//lib:unittest.bzl", "analysistest", "asserts")
load("//bzl:djinni_codegen.bzl", "DjinniInfo", "cc_djinni_library", "djinni_codegen", "djinni_library", "java_djinni_library")

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
    native.test_suite(
        name = name,
        tests = [":provider_test", ":cpp_action_test", ":java_action_test", ":ambiguous_input_test", ":invalid_include_test"],
    )
