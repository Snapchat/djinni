"""Bazel rules for running Djinni code generation."""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@rules_java//java:defs.bzl", "java_library")

DjinniInfo = provider(
    doc = "Djinni IDL provider.",
    fields = {
        "idl": "Root .djinni file.",
        "transitive_srcs": "Root IDL, imported IDL/YAML/proto sources, and transitive dependency sources.",
        "idl_include_paths": "Depset of execution-root include paths.",
        "validation_files": "Depset of IDL verification outputs.",
    },
)

def _collect(values):
    result = []
    for value in values:
        result += value
    return result

def _single_output_dir(files, name):
    if not files:
        return None
    directory = files[0].dirname
    for file in files:
        if file.dirname != directory:
            fail("%s outputs must all be in one directory; got %s and %s" % (
                name,
                directory,
                file.dirname,
            ))
    return directory

def _add_string_arg(args, flag, value):
    if value:
        args.add(flag)
        args.add(value)

def _add_bool_arg(args, flag, value):
    if value:
        args.add(flag)
        args.add(value)

def _add_label_path_arg(args, flag, file):
    if file:
        args.add(flag)
        args.add(file.path)

def _add_ident_arg(args, flag, value):
    _add_string_arg(args, flag, value)

def _djinni_codegen_impl(ctx):
    if ctx.attr.djinni:
        djinni = ctx.attr.djinni[DjinniInfo]
        idl = djinni.idl
        input_depsets = [djinni.transitive_srcs, djinni.validation_files]
        idl_include_paths = djinni.idl_include_paths.to_list() + _include_paths(ctx)
    else:
        idl = ctx.file.idl
        input_depsets = []
        idl_include_paths = _include_paths(ctx)
        if not idl:
            fail("djinni_codegen requires either idl or djinni")
    if ctx.attr.djinni and ctx.file.idl:
        fail("Specify only one of djinni or idl")

    cpp_files = ctx.outputs.cpp_srcs + ctx.outputs.cpp_hdrs
    jni_files = ctx.outputs.jni_srcs + ctx.outputs.jni_hdrs
    objc_files = ctx.outputs.objc_srcs + ctx.outputs.objc_hdrs
    objcpp_files = ctx.outputs.objcpp_srcs + ctx.outputs.objcpp_hdrs
    wasm_files = ctx.outputs.wasm_srcs + ctx.outputs.wasm_hdrs
    all_outputs = _collect([
        cpp_files,
        ctx.outputs.java_srcs,
        jni_files,
        objc_files,
        objcpp_files,
        wasm_files,
        ctx.outputs.ts_srcs,
        ctx.outputs.yaml_srcs,
    ])

    if not all_outputs:
        fail("djinni_codegen requires at least one declared output")

    args = ctx.actions.args()

    java_dir = _single_output_dir(ctx.outputs.java_srcs, "java")
    cpp_src_dir = _single_output_dir(ctx.outputs.cpp_srcs, "cpp source")
    cpp_hdr_dir = _single_output_dir(ctx.outputs.cpp_hdrs, "cpp header")
    jni_src_dir = _single_output_dir(ctx.outputs.jni_srcs, "jni source")
    jni_hdr_dir = _single_output_dir(ctx.outputs.jni_hdrs, "jni header")
    objc_dir = _single_output_dir(objc_files, "objc")
    objcpp_dir = _single_output_dir(objcpp_files, "objcpp")
    wasm_dir = _single_output_dir(wasm_files, "wasm")
    ts_dir = _single_output_dir(ctx.outputs.ts_srcs, "ts")
    yaml_dir = _single_output_dir(ctx.outputs.yaml_srcs, "yaml")

    if java_dir:
        args.add("--java-out")
        args.add(java_dir)
    _add_string_arg(args, "--java-package", ctx.attr.java_package)
    _add_string_arg(args, "--java-class-access-modifier", ctx.attr.java_class_access_modifier)
    _add_string_arg(args, "--java-nullable-annotation", ctx.attr.java_nullable_annotation)
    _add_string_arg(args, "--java-nonnull-annotation", ctx.attr.java_nonnull_annotation)
    _add_bool_arg(args, "--java-implement-android-os-parcelable", ctx.attr.java_implement_android_os_parcelable)
    _add_bool_arg(args, "--java-use-final-for-record", ctx.attr.java_use_final_for_record)
    _add_bool_arg(args, "--java-gen-interface", ctx.attr.java_gen_interface)

    if cpp_src_dir or cpp_hdr_dir:
        cpp_out_dir = cpp_src_dir or cpp_hdr_dir
        args.add("--cpp-out")
        args.add(cpp_out_dir)
        if cpp_hdr_dir and cpp_hdr_dir != cpp_out_dir:
            args.add("--cpp-header-out")
            args.add(cpp_hdr_dir)
    _add_string_arg(args, "--cpp-namespace", ctx.attr.cpp_namespace)
    _add_string_arg(args, "--cpp-include-prefix", ctx.attr.cpp_include_prefix)
    _add_string_arg(args, "--cpp-base-lib-include-prefix", ctx.attr.cpp_base_lib_include_prefix)
    _add_string_arg(args, "--cpp-optional-template", ctx.attr.cpp_optional_template)
    _add_string_arg(args, "--cpp-optional-header", ctx.attr.cpp_optional_header)
    _add_string_arg(args, "--cpp-extended-record-include-prefix", ctx.attr.cpp_extended_record_include_prefix)
    _add_bool_arg(args, "--cpp-use-wide-strings", ctx.attr.cpp_use_wide_strings)

    if jni_src_dir or jni_hdr_dir:
        jni_out_dir = jni_src_dir or jni_hdr_dir
        args.add("--jni-out")
        args.add(jni_out_dir)
        if jni_hdr_dir and jni_hdr_dir != jni_out_dir:
            args.add("--jni-header-out")
            args.add(jni_hdr_dir)
    _add_string_arg(args, "--jni-namespace", ctx.attr.jni_namespace)
    _add_string_arg(args, "--jni-include-prefix", ctx.attr.jni_include_prefix)
    _add_string_arg(args, "--jni-include-cpp-prefix", ctx.attr.jni_include_cpp_prefix)
    _add_string_arg(args, "--jni-base-lib-include-prefix", ctx.attr.jni_base_lib_include_prefix)
    _add_bool_arg(args, "--jni-use-on-load-initializer", ctx.attr.jni_use_on_load_initializer)
    _add_label_path_arg(args, "--jni-function-prologue-file", ctx.file.jni_function_prologue_file)

    if objc_dir:
        args.add("--objc-out")
        args.add(objc_dir)
    _add_string_arg(args, "--objc-type-prefix", ctx.attr.objc_type_prefix)
    _add_string_arg(args, "--objc-include-prefix", ctx.attr.objc_include_prefix)
    _add_string_arg(args, "--objc-extended-record-include-prefix", ctx.attr.objc_extended_record_include_prefix)
    _add_string_arg(args, "--objc-swift-bridging-header", ctx.attr.objc_swift_bridging_header)
    _add_bool_arg(args, "--objc-gen-protocol", ctx.attr.objc_gen_protocol)
    _add_bool_arg(args, "--objc-disable-class-ctor", ctx.attr.objc_disable_class_ctor)
    _add_bool_arg(args, "--objc-closed-enums", ctx.attr.objc_closed_enums)
    _add_bool_arg(args, "--objc-strict-protocols", ctx.attr.objc_strict_protocols)

    if objcpp_dir:
        args.add("--objcpp-out")
        args.add(objcpp_dir)
    _add_string_arg(args, "--objcpp-namespace", ctx.attr.objcpp_namespace)
    _add_string_arg(args, "--objcpp-include-prefix", ctx.attr.objcpp_include_prefix)
    _add_string_arg(args, "--objcpp-include-cpp-prefix", ctx.attr.objcpp_include_cpp_prefix)
    _add_string_arg(args, "--objcpp-include-objc-prefix", ctx.attr.objcpp_include_objc_prefix)
    _add_label_path_arg(args, "--objcpp-function-prologue-file", ctx.file.objcpp_function_prologue_file)
    _add_bool_arg(args, "--objcpp-disable-exception-translation", ctx.attr.objcpp_disable_exception_translation)

    if wasm_dir:
        args.add("--wasm-out")
        args.add(wasm_dir)
    _add_string_arg(args, "--wasm-namespace", ctx.attr.wasm_namespace)
    _add_string_arg(args, "--wasm-include-prefix", ctx.attr.wasm_include_prefix)
    _add_string_arg(args, "--wasm-include-cpp-prefix", ctx.attr.wasm_include_cpp_prefix)
    _add_string_arg(args, "--wasm-base-lib-include-prefix", ctx.attr.wasm_base_lib_include_prefix)
    _add_bool_arg(args, "--wasm-omit-constants", ctx.attr.wasm_omit_constants)
    _add_bool_arg(args, "--wasm-omit-namespace-alias", ctx.attr.wasm_omit_namespace_alias)

    if ts_dir:
        args.add("--ts-out")
        args.add(ts_dir)
    _add_string_arg(args, "--ts-module", ctx.attr.ts_module)

    if yaml_dir:
        args.add("--yaml-out")
        args.add(yaml_dir)
    _add_string_arg(args, "--yaml-out-file", ctx.attr.yaml_out_file)
    _add_string_arg(args, "--yaml-prefix", ctx.attr.yaml_prefix)

    _add_ident_arg(args, "--ident-java-enum", ctx.attr.ident_java_enum)
    _add_ident_arg(args, "--ident-java-field", ctx.attr.ident_java_field)
    _add_ident_arg(args, "--ident-java-type", ctx.attr.ident_java_type)
    _add_ident_arg(args, "--ident-cpp-enum", ctx.attr.ident_cpp_enum)
    _add_ident_arg(args, "--ident-cpp-field", ctx.attr.ident_cpp_field)
    _add_ident_arg(args, "--ident-cpp-method", ctx.attr.ident_cpp_method)
    _add_ident_arg(args, "--ident-cpp-type", ctx.attr.ident_cpp_type)
    _add_ident_arg(args, "--ident-cpp-enum-type", ctx.attr.ident_cpp_enum_type)
    _add_ident_arg(args, "--ident-cpp-type-param", ctx.attr.ident_cpp_type_param)
    _add_ident_arg(args, "--ident-cpp-local", ctx.attr.ident_cpp_local)
    _add_ident_arg(args, "--ident-cpp-file", ctx.attr.ident_cpp_file)
    _add_ident_arg(args, "--ident-jni-class", ctx.attr.ident_jni_class)
    _add_ident_arg(args, "--ident-jni-file", ctx.attr.ident_jni_file)
    _add_ident_arg(args, "--ident-objc-enum", ctx.attr.ident_objc_enum)
    _add_ident_arg(args, "--ident-objc-field", ctx.attr.ident_objc_field)
    _add_ident_arg(args, "--ident-objc-method", ctx.attr.ident_objc_method)
    _add_ident_arg(args, "--ident-objc-type", ctx.attr.ident_objc_type)
    _add_ident_arg(args, "--ident-objc-type-param", ctx.attr.ident_objc_type_param)
    _add_ident_arg(args, "--ident-objc-local", ctx.attr.ident_objc_local)
    _add_ident_arg(args, "--ident-objc-const", ctx.attr.ident_objc_const)
    _add_ident_arg(args, "--ident-objc-file", ctx.attr.ident_objc_file)

    args.add("--idl")
    args.add(idl.path)
    for include_path in idl_include_paths:
        args.add("--idl-include-path")
        args.add(include_path)

    inputs = depset(
        direct = ([idl] if not ctx.attr.djinni else []) + ctx.files.srcs + [
            file
            for file in [
                ctx.file.jni_function_prologue_file,
                ctx.file.objcpp_function_prologue_file,
            ]
            if file
        ],
        transitive = input_depsets,
    )

    ctx.actions.run(
        executable = ctx.attr.compiler[DefaultInfo].files_to_run,
        arguments = [args],
        inputs = inputs,
        outputs = all_outputs,
        mnemonic = "DjinniCodegen",
        progress_message = "Generating Djinni sources for %s" % ctx.label,
    )

    return [
        DefaultInfo(files = depset(all_outputs)),
        OutputGroupInfo(
            all = depset(all_outputs),
            cpp_srcs = depset(ctx.outputs.cpp_srcs),
            cpp_hdrs = depset(ctx.outputs.cpp_hdrs),
            java_srcs = depset(ctx.outputs.java_srcs),
            jni_srcs = depset(ctx.outputs.jni_srcs),
            jni_hdrs = depset(ctx.outputs.jni_hdrs),
            objc_srcs = depset(ctx.outputs.objc_srcs),
            objc_hdrs = depset(ctx.outputs.objc_hdrs),
            objcpp_srcs = depset(ctx.outputs.objcpp_srcs),
            objcpp_hdrs = depset(ctx.outputs.objcpp_hdrs),
            wasm_srcs = depset(ctx.outputs.wasm_srcs),
            wasm_hdrs = depset(ctx.outputs.wasm_hdrs),
            ts_srcs = depset(ctx.outputs.ts_srcs),
            yaml_srcs = depset(ctx.outputs.yaml_srcs),
        ),
    ]

_djinni_codegen = rule(
    implementation = _djinni_codegen_impl,
    attrs = {
        "djinni": attr.label(providers = [DjinniInfo]),
        "idl": attr.label(allow_single_file = [".djinni"]),
        "srcs": attr.label_list(allow_files = [".djinni", ".yaml", ".yml", ".proto"]),
        "idl_include_paths": attr.string_list(),
        "cpp_srcs": attr.output_list(),
        "cpp_hdrs": attr.output_list(),
        "java_srcs": attr.output_list(),
        "jni_srcs": attr.output_list(),
        "jni_hdrs": attr.output_list(),
        "objc_srcs": attr.output_list(),
        "objc_hdrs": attr.output_list(),
        "objcpp_srcs": attr.output_list(),
        "objcpp_hdrs": attr.output_list(),
        "wasm_srcs": attr.output_list(),
        "wasm_hdrs": attr.output_list(),
        "ts_srcs": attr.output_list(),
        "yaml_srcs": attr.output_list(),
        "java_package": attr.string(),
        "java_class_access_modifier": attr.string(),
        "java_nullable_annotation": attr.string(),
        "java_nonnull_annotation": attr.string(),
        "java_implement_android_os_parcelable": attr.string(),
        "java_use_final_for_record": attr.string(),
        "java_gen_interface": attr.string(),
        "cpp_namespace": attr.string(),
        "cpp_include_prefix": attr.string(),
        "cpp_base_lib_include_prefix": attr.string(),
        "cpp_optional_template": attr.string(),
        "cpp_optional_header": attr.string(),
        "cpp_extended_record_include_prefix": attr.string(),
        "cpp_use_wide_strings": attr.string(),
        "jni_namespace": attr.string(),
        "jni_include_prefix": attr.string(),
        "jni_include_cpp_prefix": attr.string(),
        "jni_base_lib_include_prefix": attr.string(),
        "jni_use_on_load_initializer": attr.string(),
        "jni_function_prologue_file": attr.label(allow_single_file = True),
        "objc_type_prefix": attr.string(),
        "objc_include_prefix": attr.string(),
        "objc_extended_record_include_prefix": attr.string(),
        "objc_swift_bridging_header": attr.string(),
        "objc_gen_protocol": attr.string(),
        "objc_disable_class_ctor": attr.string(),
        "objc_closed_enums": attr.string(),
        "objc_strict_protocols": attr.string(),
        "objcpp_namespace": attr.string(),
        "objcpp_include_prefix": attr.string(),
        "objcpp_include_cpp_prefix": attr.string(),
        "objcpp_include_objc_prefix": attr.string(),
        "objcpp_function_prologue_file": attr.label(allow_single_file = True),
        "objcpp_disable_exception_translation": attr.string(),
        "wasm_namespace": attr.string(),
        "wasm_include_prefix": attr.string(),
        "wasm_include_cpp_prefix": attr.string(),
        "wasm_base_lib_include_prefix": attr.string(),
        "wasm_omit_constants": attr.string(),
        "wasm_omit_namespace_alias": attr.string(),
        "ts_module": attr.string(),
        "yaml_out_file": attr.string(),
        "yaml_prefix": attr.string(),
        "ident_java_enum": attr.string(),
        "ident_java_field": attr.string(),
        "ident_java_type": attr.string(),
        "ident_cpp_enum": attr.string(),
        "ident_cpp_field": attr.string(),
        "ident_cpp_method": attr.string(),
        "ident_cpp_type": attr.string(),
        "ident_cpp_enum_type": attr.string(),
        "ident_cpp_type_param": attr.string(),
        "ident_cpp_local": attr.string(),
        "ident_cpp_file": attr.string(),
        "ident_jni_class": attr.string(),
        "ident_jni_file": attr.string(),
        "ident_objc_enum": attr.string(),
        "ident_objc_field": attr.string(),
        "ident_objc_method": attr.string(),
        "ident_objc_type": attr.string(),
        "ident_objc_type_param": attr.string(),
        "ident_objc_local": attr.string(),
        "ident_objc_const": attr.string(),
        "ident_objc_file": attr.string(),
        "compiler": attr.label(
            default = Label("//src:djinni"),
            executable = True,
            cfg = "exec",
        ),
    },
)

def _include_paths(ctx):
    # Include paths are repository-relative, never relative to the consuming workspace.
    root = ctx.label.workspace_root
    result = []
    for path in ctx.attr.idl_include_paths:
        if path.startswith("/") or ".." in path.split("/"):
            fail("idl_include_paths must be repository-relative: %s" % path)
        result.append(root + "/" + path if root and path else root or path or ".")
    return result

def _djinni_library_impl(ctx):
    transitive = [
        dep[DjinniInfo].transitive_srcs
        for dep in ctx.attr.deps
    ]
    include_paths = depset(
        direct = _include_paths(ctx),
        transitive = [dep[DjinniInfo].idl_include_paths for dep in ctx.attr.deps],
    )

    transitive_srcs = depset(
        direct = [ctx.file.idl] + ctx.files.srcs,
        transitive = transitive,
    )

    validation = []
    if ctx.attr.verify:
        validated_inputs = ctx.actions.declare_file(ctx.label.name + ".djinni.inputs")
        args = ctx.actions.args()
        args.add("--idl", ctx.file.idl)
        args.add("--skip-generation", "true")
        args.add("--list-in-files", validated_inputs)
        args.add_all(include_paths, before_each = "--idl-include-path")
        ctx.actions.run(
            executable = ctx.attr.compiler[DefaultInfo].files_to_run,
            arguments = [args],
            inputs = transitive_srcs,
            outputs = [validated_inputs],
            mnemonic = "DjinniVerify",
            progress_message = "Verifying Djinni IDL for %s" % ctx.label,
        )
        validation.append(validated_inputs)
    validation_files = depset(
        direct = validation,
        transitive = [dep[DjinniInfo].validation_files for dep in ctx.attr.deps],
    )
    return [
        DefaultInfo(files = depset([ctx.file.idl] + ctx.files.srcs, transitive = [validation_files])),
        DjinniInfo(
            idl = ctx.file.idl,
            transitive_srcs = transitive_srcs,
            idl_include_paths = include_paths,
            validation_files = validation_files,
        ),
    ]

djinni_library = rule(
    doc = "Declares one root IDL and its import closure; verifies syntax and types by default.",
    implementation = _djinni_library_impl,
    attrs = {
        "idl": attr.label(allow_single_file = [".djinni"], mandatory = True),
        "srcs": attr.label_list(allow_files = [".djinni", ".yaml", ".yml", ".proto"]),
        "deps": attr.label_list(providers = [DjinniInfo]),
        "idl_include_paths": attr.string_list(),
        "verify": attr.bool(default = True),
        "compiler": attr.label(default = Label("//src:djinni"), executable = True, cfg = "exec"),
    },
)

def _outs(outs, key):
    return outs.get(key, [])

_BOOL_ATTRS = [
    "java_implement_android_os_parcelable",
    "java_use_final_for_record",
    "java_gen_interface",
    "cpp_use_wide_strings",
    "jni_use_on_load_initializer",
    "objc_gen_protocol",
    "objc_disable_class_ctor",
    "objc_closed_enums",
    "objc_strict_protocols",
    "objcpp_disable_exception_translation",
    "wasm_omit_constants",
    "wasm_omit_namespace_alias",
]

def _normalize_kwargs(kwargs):
    normalized = dict(kwargs)
    for name in _BOOL_ATTRS:
        if name in normalized:
            value = normalized[name]
            if type(value) != "bool":
                fail("%s must be a boolean" % name)
            normalized[name] = "true" if value else "false"
    return normalized

def djinni_codegen(name, outs, **kwargs):
    """Runs Djinni and exposes generated files through output groups.

    Args:
        name: Rule name.
        outs: Dictionary keyed by output category. Supported keys are
            cpp_srcs, cpp_hdrs, java_srcs, jni_srcs, jni_hdrs, objc_srcs,
            objc_hdrs, objcpp_srcs, objcpp_hdrs, wasm_srcs, wasm_hdrs,
            ts_srcs, and yaml_srcs.
        **kwargs: Djinni options and input labels.
    """
    unknown = [key for key in outs if key not in _OUTPUT_KEYS]
    if unknown:
        fail("Unknown Djinni output categories: %s" % unknown)
    normalized = _normalize_kwargs(kwargs)
    _djinni_codegen(
        name = name,
        cpp_srcs = _outs(outs, "cpp_srcs"),
        cpp_hdrs = _outs(outs, "cpp_hdrs"),
        java_srcs = _outs(outs, "java_srcs"),
        jni_srcs = _outs(outs, "jni_srcs"),
        jni_hdrs = _outs(outs, "jni_hdrs"),
        objc_srcs = _outs(outs, "objc_srcs"),
        objc_hdrs = _outs(outs, "objc_hdrs"),
        objcpp_srcs = _outs(outs, "objcpp_srcs"),
        objcpp_hdrs = _outs(outs, "objcpp_hdrs"),
        wasm_srcs = _outs(outs, "wasm_srcs"),
        wasm_hdrs = _outs(outs, "wasm_hdrs"),
        ts_srcs = _outs(outs, "ts_srcs"),
        yaml_srcs = _outs(outs, "yaml_srcs"),
        **normalized
    )

def _exactly_one_dep(name, deps):
    if len(deps) != 1:
        fail("%s expects exactly one djinni_library dep" % name)

_OUTPUT_KEYS = [
    "cpp_srcs",
    "cpp_hdrs",
    "java_srcs",
    "jni_srcs",
    "jni_hdrs",
    "objc_srcs",
    "objc_hdrs",
    "objcpp_srcs",
    "objcpp_hdrs",
    "wasm_srcs",
    "wasm_hdrs",
    "ts_srcs",
    "yaml_srcs",
]

_COMMON_ATTRS = ["testonly", "tags", "compatible_with", "restricted_to", "target_compatible_with", "features", "deprecation"]

def _language_codegen(name, deps, outs, language, kwargs):
    _exactly_one_dep(name, deps)
    unknown = [key for key in outs if key not in _OUTPUT_KEYS]
    if unknown:
        fail("Unknown Djinni output categories: %s" % unknown)
    common = {}
    options = dict(kwargs)
    for attr_name in _COMMON_ATTRS:
        if attr_name in options:
            common[attr_name] = options.pop(attr_name)
    selected = {key: value for key, value in outs.items() if key.startswith(language + "_")}
    djinni_codegen(
        name = name,
        djinni = deps[0],
        outs = selected,
        visibility = ["//visibility:private"],
        **dict(options, **common)
    )
    return common

def _consumer_attrs(library_kwargs, common):
    attrs = dict(library_kwargs)
    for key, value in common.items():
        if key in attrs:
            fail("Pass common attribute %s on the macro, not in library_kwargs" % key)
        attrs[key] = value
    return attrs

def cc_djinni_library(
        name,
        deps,
        outs,
        srcs = [],
        hdrs = [],
        includes = [],
        cc_deps = [],
        copts = [],
        linkopts = [],
        alwayslink = 0,
        visibility = None,
        library_kwargs = {},
        **kwargs):
    """Generates C++ from a djinni_library and wraps it in cc_library."""
    codegen_name = name + "__djinni_cpp_codegen"
    common = _language_codegen(codegen_name, deps, outs, "cpp", kwargs)
    cc_library(
        name = name,
        srcs = _outs(outs, "cpp_srcs") + srcs,
        hdrs = _outs(outs, "cpp_hdrs") + hdrs,
        includes = includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def java_djinni_library(
        name,
        deps,
        outs,
        java_deps = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates Java from a djinni_library and wraps it in java_library."""
    codegen_name = name + "__djinni_java_codegen"
    common = _language_codegen(codegen_name, deps, outs, "java", kwargs)
    java_library(
        name = name,
        srcs = _outs(outs, "java_srcs") + srcs,
        deps = java_deps,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def jni_djinni_library(
        name,
        deps,
        outs,
        includes = [],
        cc_deps = [],
        copts = [],
        linkopts = [],
        alwayslink = 0,
        visibility = None,
        srcs = [],
        hdrs = [],
        library_kwargs = {},
        **kwargs):
    """Generates JNI C++ from a djinni_library and wraps it in cc_library."""
    codegen_name = name + "__djinni_jni_codegen"
    common = _language_codegen(codegen_name, deps, outs, "jni", kwargs)
    cc_library(
        name = name,
        srcs = _outs(outs, "jni_srcs") + srcs,
        hdrs = _outs(outs, "jni_hdrs") + hdrs,
        includes = includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def wasm_djinni_cc_binary(
        name,
        deps,
        outs,
        cc_deps = [],
        copts = [],
        linkopts = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates WASM C++ bridge sources from a djinni_library and wraps them in cc_binary."""
    codegen_name = name + "__djinni_wasm_codegen"
    common = _language_codegen(codegen_name, deps, outs, "wasm", kwargs)
    cc_binary(
        name = name,
        srcs = _outs(outs, "wasm_srcs") + _outs(outs, "wasm_hdrs") + srcs,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )
