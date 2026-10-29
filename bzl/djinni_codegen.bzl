"""Bazel rules for running Djinni code generation."""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")

DjinniInfo = provider(
    doc = "Djinni IDL provider.",
    fields = {
        "idl": "Root .djinni file.",
        "transitive_srcs": "Root IDL, imported IDL/YAML/proto sources, and transitive dependency sources.",
        "idl_include_paths": "Depset of execution-root include paths.",
        "validation_files": "Depset of IDL verification outputs.",
        "declarations_manifest": "Resolved internal declarations produced by Djinni.",
        "project_name": "Consumer project name for language-specific defaults.",
        "cpp_namespace": "Default C++ namespace.",
        "cpp_include_prefix": "Default generated C++ include prefix.",
        "cpp_base_lib_include_prefix": "Default C++ runtime include prefix.",
        "cpp_optional_template": "Default C++ optional template.",
        "cpp_optional_header": "Default C++ optional header.",
        "defaults_file": "Organization-wide default generation configuration YAML.",
        "config_file": "IDL-specific generation configuration YAML, applied after defaults.",
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

def _add_ident_arg(args, flag, value):
    _add_string_arg(args, flag, value)

def _djinni_codegen_impl(ctx):
    if ctx.attr.djinni:
        djinni = ctx.attr.djinni[DjinniInfo]
        if ctx.attr.project_name and djinni.project_name and ctx.attr.project_name != djinni.project_name:
            fail("project_name %s does not match djinni_library project_name %s" % (ctx.attr.project_name, djinni.project_name))
        idl = djinni.idl
        input_depsets = [djinni.transitive_srcs, djinni.validation_files, depset([djinni.declarations_manifest])]
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
    c_files = ctx.outputs.c_srcs + ctx.outputs.c_hdrs
    jni_files = ctx.outputs.jni_srcs + ctx.outputs.jni_hdrs
    objc_files = ctx.outputs.objc_srcs + ctx.outputs.objc_hdrs
    objcpp_files = ctx.outputs.objcpp_srcs + ctx.outputs.objcpp_hdrs
    wasm_files = ctx.outputs.wasm_srcs + ctx.outputs.wasm_hdrs
    all_outputs = _collect([
        cpp_files,
        c_files,
        ctx.outputs.java_srcs,
        jni_files,
        objc_files,
        objcpp_files,
        wasm_files,
        ctx.outputs.ts_srcs,
    ])

    tree_outputs = {}
    for language in ctx.attr.tree_languages:
        if language not in ["cpp", "c", "java", "jni", "objc", "wasm", "ts"]:
            fail("Unsupported tree output language: %s" % language)
        tree_outputs[language] = ctx.actions.declare_directory(ctx.label.name + "." + language)
        all_outputs.append(tree_outputs[language])
    for language in tree_outputs:
        if language == "cpp" and cpp_files or language == "c" and c_files or language == "java" and ctx.outputs.java_srcs or language == "jni" and jni_files or language == "objc" and (objc_files or objcpp_files) or language == "wasm" and wasm_files or language == "ts" and ctx.outputs.ts_srcs:
            fail("Do not combine tree output and explicit outputs for %s" % language)

    if tree_outputs:
        manifest = ctx.actions.declare_file(ctx.label.name + ".outputs")
        all_outputs.append(manifest)

    if not all_outputs:
        fail("djinni_codegen requires at least one declared output")

    args = ctx.actions.args()
    defaults_file = djinni.defaults_file if ctx.attr.djinni else ctx.file.defaults_file
    config_file = djinni.config_file if ctx.attr.djinni else ctx.file.config_file
    config_files = ([defaults_file] if defaults_file else []) + ([config_file] if config_file else [])

    java_package_path = ctx.attr.java_package.replace(".", "/")
    java_dir = (tree_outputs["java"].path + "/" + java_package_path if java_package_path else tree_outputs["java"].path) if "java" in tree_outputs else _single_output_dir(ctx.outputs.java_srcs, "java")
    cpp_prefix = ctx.attr.cpp_include_prefix or (ctx.attr.djinni[DjinniInfo].cpp_include_prefix if ctx.attr.djinni else "")
    cpp_tree_dir = tree_outputs["cpp"].path + "/" + cpp_prefix.rstrip("/") if "cpp" in tree_outputs and cpp_prefix else (tree_outputs["cpp"].path if "cpp" in tree_outputs else None)
    cpp_src_dir = cpp_tree_dir if cpp_tree_dir else _single_output_dir(ctx.outputs.cpp_srcs, "cpp source")
    cpp_hdr_dir = cpp_tree_dir if cpp_tree_dir else _single_output_dir(ctx.outputs.cpp_hdrs, "cpp header")
    c_prefix = ctx.attr.c_include_prefix.rstrip("/")
    c_tree_dir = tree_outputs["c"].path + "/" + c_prefix if "c" in tree_outputs and c_prefix else (tree_outputs["c"].path if "c" in tree_outputs else None)
    c_src_dir = tree_outputs["c"].path if "c" in tree_outputs else _single_output_dir(ctx.outputs.c_srcs, "C wrapper source")
    c_hdr_dir = c_tree_dir if c_tree_dir else _single_output_dir(ctx.outputs.c_hdrs, "C header")
    jni_prefix = ctx.attr.jni_include_prefix.rstrip("/")
    jni_tree_dir = tree_outputs["jni"].path + "/" + jni_prefix if "jni" in tree_outputs and jni_prefix else (tree_outputs["jni"].path if "jni" in tree_outputs else None)
    jni_src_dir = jni_tree_dir if jni_tree_dir else _single_output_dir(ctx.outputs.jni_srcs, "jni source")
    jni_hdr_dir = jni_tree_dir if jni_tree_dir else _single_output_dir(ctx.outputs.jni_hdrs, "jni header")
    objc_prefix = ctx.attr.objc_include_prefix.rstrip("/")
    objc_tree_dir = tree_outputs["objc"].path + "/" + objc_prefix if "objc" in tree_outputs and objc_prefix else (tree_outputs["objc"].path if "objc" in tree_outputs else None)
    objc_dir = objc_tree_dir if objc_tree_dir else _single_output_dir(objc_files, "objc")
    objcpp_dir = objc_tree_dir if objc_tree_dir else _single_output_dir(objcpp_files, "objcpp")
    wasm_prefix = ctx.attr.wasm_include_prefix.rstrip("/")
    wasm_tree_dir = tree_outputs["wasm"].path + "/" + wasm_prefix if "wasm" in tree_outputs and wasm_prefix else (tree_outputs["wasm"].path if "wasm" in tree_outputs else None)
    wasm_dir = wasm_tree_dir if wasm_tree_dir else _single_output_dir(wasm_files, "wasm")
    ts_dir = tree_outputs["ts"].path if "ts" in tree_outputs else _single_output_dir(ctx.outputs.ts_srcs, "ts")

    if java_dir:
        args.add("--java-out")
        args.add(java_dir)
    if not config_file:
        _add_string_arg(args, "--java-package", ctx.attr.java_package)
        _add_string_arg(args, "--java-class-access-modifier", ctx.attr.java_class_access_modifier)
        _add_string_arg(args, "--java-nullable-annotation", ctx.attr.java_nullable_annotation)
        _add_string_arg(args, "--java-nonnull-annotation", ctx.attr.java_nonnull_annotation)
        _add_bool_arg(args, "--java-implement-android-os-parcelable", ctx.attr.java_implement_android_os_parcelable)
        _add_bool_arg(args, "--java-use-final-for-record", ctx.attr.java_use_final_for_record)
        _add_bool_arg(args, "--java-gen-interface", ctx.attr.java_gen_interface)
        _add_bool_arg(args, "--java-legacy-records", ctx.attr.java_legacy_records)

    if cpp_src_dir or cpp_hdr_dir:
        cpp_out_dir = cpp_src_dir or cpp_hdr_dir
        args.add("--cpp-out")
        args.add(cpp_out_dir)
        if cpp_hdr_dir and cpp_hdr_dir != cpp_out_dir:
            args.add("--cpp-header-out")
            args.add(cpp_hdr_dir)
    idl_defaults = ctx.attr.djinni[DjinniInfo] if ctx.attr.djinni else None
    if not config_file:
        _add_string_arg(args, "--cpp-namespace", _cpp_string_option(ctx.attr.cpp_namespace, idl_defaults.cpp_namespace if idl_defaults else ""))
        _add_string_arg(args, "--cpp-include-prefix", _cpp_string_option(ctx.attr.cpp_include_prefix, idl_defaults.cpp_include_prefix if idl_defaults else ""))
        _add_string_arg(args, "--cpp-base-lib-include-prefix", _cpp_string_option(ctx.attr.cpp_base_lib_include_prefix, idl_defaults.cpp_base_lib_include_prefix if idl_defaults else ""))
        _add_string_arg(args, "--cpp-optional-template", _cpp_string_option(ctx.attr.cpp_optional_template, idl_defaults.cpp_optional_template if idl_defaults else ""))
        _add_string_arg(args, "--cpp-optional-header", _cpp_string_option(ctx.attr.cpp_optional_header, idl_defaults.cpp_optional_header if idl_defaults else ""))
        _add_string_arg(args, "--cpp-extended-record-include-prefix", ctx.attr.cpp_extended_record_include_prefix)
        _add_bool_arg(args, "--cpp-use-wide-strings", ctx.attr.cpp_use_wide_strings)
        _add_bool_arg(args, "--cpp-legacy-records", ctx.attr.cpp_legacy_records)

    if c_src_dir or c_hdr_dir:
        args.add("--c-out", c_src_dir or c_hdr_dir)
        if c_hdr_dir and c_hdr_dir != (c_src_dir or c_hdr_dir):
            args.add("--c-header-out", c_hdr_dir)
    if not config_file:
        _add_string_arg(args, "--c-namespace", ctx.attr.c_namespace)
        _add_string_arg(args, "--c-include-prefix", ctx.attr.c_include_prefix)
        _add_string_arg(args, "--c-base-lib-include-prefix", ctx.attr.c_base_lib_include_prefix)
        _add_string_arg(args, "--c-wrapper-cpp-namespace", ctx.attr.c_wrapper_cpp_namespace)
        _add_bool_arg(args, "--c-wrapper-use-dlsym", ctx.attr.c_wrapper_use_dlsym)

    if jni_src_dir or jni_hdr_dir:
        jni_out_dir = jni_src_dir or jni_hdr_dir
        args.add("--jni-out")
        args.add(jni_out_dir)
        if jni_hdr_dir and jni_hdr_dir != jni_out_dir:
            args.add("--jni-header-out")
            args.add(jni_hdr_dir)
    if not config_file:
        _add_string_arg(args, "--jni-namespace", ctx.attr.jni_namespace)
        _add_string_arg(args, "--jni-include-prefix", ctx.attr.jni_include_prefix)
        _add_string_arg(args, "--jni-include-cpp-prefix", ctx.attr.jni_include_cpp_prefix)
        _add_string_arg(args, "--jni-base-lib-include-prefix", ctx.attr.jni_base_lib_include_prefix)
        _add_bool_arg(args, "--jni-use-on-load-initializer", ctx.attr.jni_use_on_load_initializer)
        _add_string_arg(args, "--jni-function-prologue-file", ctx.attr.jni_function_prologue_file)

    if objc_dir:
        args.add("--objc-out")
        args.add(objc_dir)
    if not config_file:
        _add_string_arg(args, "--objc-type-prefix", ctx.attr.objc_type_prefix)
        _add_string_arg(args, "--objc-include-prefix", ctx.attr.objc_include_prefix)
        _add_string_arg(args, "--objc-base-lib-include-prefix", ctx.attr.objc_base_lib_include_prefix)
        _add_string_arg(args, "--objc-extended-record-include-prefix", ctx.attr.objc_extended_record_include_prefix)
        _add_string_arg(args, "--objc-swift-bridging-header", ctx.attr.objc_swift_bridging_header)
        _add_bool_arg(args, "--objc-gen-protocol", ctx.attr.objc_gen_protocol)
        _add_bool_arg(args, "--objc-disable-class-ctor", ctx.attr.objc_disable_class_ctor)
        _add_bool_arg(args, "--objc-closed-enums", ctx.attr.objc_closed_enums)
        _add_bool_arg(args, "--objc-strict-protocols", ctx.attr.objc_strict_protocols)
        _add_bool_arg(args, "--objc-legacy-records", ctx.attr.objc_legacy_records)

    if objcpp_dir:
        args.add("--objcpp-out")
        args.add(objcpp_dir)
    if not config_file:
        _add_string_arg(args, "--objcpp-namespace", ctx.attr.objcpp_namespace)
        _add_string_arg(args, "--objcpp-include-prefix", ctx.attr.objcpp_include_prefix)
        _add_string_arg(args, "--objcpp-include-cpp-prefix", ctx.attr.objcpp_include_cpp_prefix)
        _add_string_arg(args, "--objcpp-include-objc-prefix", ctx.attr.objcpp_include_objc_prefix)
        _add_string_arg(args, "--objcpp-function-prologue-file", ctx.attr.objcpp_function_prologue_file)
        _add_bool_arg(args, "--objcpp-disable-exception-translation", ctx.attr.objcpp_disable_exception_translation)

    if wasm_dir:
        args.add("--wasm-out")
        args.add(wasm_dir)
    if not config_file:
        _add_string_arg(args, "--wasm-namespace", ctx.attr.wasm_namespace)
        _add_string_arg(args, "--wasm-cpp-namespace", ctx.attr.wasm_cpp_namespace)
        _add_string_arg(args, "--wasm-include-prefix", ctx.attr.wasm_include_prefix)
        _add_string_arg(args, "--wasm-include-cpp-prefix", ctx.attr.wasm_include_cpp_prefix)
        _add_string_arg(args, "--wasm-base-lib-include-prefix", ctx.attr.wasm_base_lib_include_prefix)
        _add_bool_arg(args, "--wasm-omit-constants", ctx.attr.wasm_omit_constants)
        _add_bool_arg(args, "--wasm-omit-namespace-alias", ctx.attr.wasm_omit_namespace_alias)

    if ts_dir:
        args.add("--ts-out")
        args.add(ts_dir)
    if not config_file:
        _add_string_arg(args, "--ts-module", ctx.attr.ts_module)

    if not config_file:
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
        _add_ident_arg(args, "--ident-wasm-class", ctx.attr.ident_wasm_class)
        _add_ident_arg(args, "--ident-wasm-file", ctx.attr.ident_wasm_file)
        _add_ident_arg(args, "--ident-objc-enum", ctx.attr.ident_objc_enum)
        _add_ident_arg(args, "--ident-objc-field", ctx.attr.ident_objc_field)
        _add_ident_arg(args, "--ident-objc-method", ctx.attr.ident_objc_method)
        _add_ident_arg(args, "--ident-objc-type", ctx.attr.ident_objc_type)
        _add_ident_arg(args, "--ident-objc-type-param", ctx.attr.ident_objc_type_param)
        _add_ident_arg(args, "--ident-objc-local", ctx.attr.ident_objc_local)
        _add_ident_arg(args, "--ident-objc-const", ctx.attr.ident_objc_const)
        _add_ident_arg(args, "--ident-objc-file", ctx.attr.ident_objc_file)

    if defaults_file:
        args.add("--defaults")
        args.add(defaults_file.path)
    if config_file:
        args.add("--config")
        args.add(config_file.path)

    args.add("--idl")
    args.add(idl.path)
    if ctx.attr.djinni:
        args.add("--verify-declarations")
        args.add(djinni.declarations_manifest.path)
    if tree_outputs:
        args.add("--list-out-files")
        args.add(manifest.path)
    for include_path in idl_include_paths:
        args.add("--idl-include-path")
        args.add(include_path)

    inputs = depset(
        direct = ([idl] if not ctx.attr.djinni else []) + ctx.files.srcs + ctx.files.idl_include_files + ([] if ctx.attr.djinni else config_files),
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
            manifest = depset([manifest] if tree_outputs else []),
            cpp_tree = depset([tree_outputs["cpp"]] if "cpp" in tree_outputs else []),
            java_tree = depset([tree_outputs["java"]] if "java" in tree_outputs else []),
            c_tree = depset([tree_outputs["c"]] if "c" in tree_outputs else []),
            jni_tree = depset([tree_outputs["jni"]] if "jni" in tree_outputs else []),
            objc_tree = depset([tree_outputs["objc"]] if "objc" in tree_outputs else []),
            wasm_tree = depset([tree_outputs["wasm"]] if "wasm" in tree_outputs else []),
            ts_tree = depset([tree_outputs["ts"]] if "ts" in tree_outputs else []),
            cpp_srcs = depset(ctx.outputs.cpp_srcs),
            cpp_hdrs = depset(ctx.outputs.cpp_hdrs),
            c_srcs = depset(ctx.outputs.c_srcs),
            c_hdrs = depset(ctx.outputs.c_hdrs),
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
        ),
    ]

_djinni_codegen = rule(
    implementation = _djinni_codegen_impl,
    attrs = {
        "djinni": attr.label(providers = [DjinniInfo]),
        "project_name": attr.string(),
        "idl": attr.label(allow_single_file = [".djinni"]),
        "srcs": attr.label_list(allow_files = [".djinni", ".yaml", ".yml", ".proto"]),
        "defaults_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "config_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "idl_include_paths": attr.string_list(),
        "idl_include_files": attr.label_list(allow_files = True),
        "tree_languages": attr.string_list(),
        "cpp_srcs": attr.output_list(),
        "cpp_hdrs": attr.output_list(),
        "c_srcs": attr.output_list(),
        "c_hdrs": attr.output_list(),
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
        "java_package": attr.string(),
        "java_class_access_modifier": attr.string(),
        "java_nullable_annotation": attr.string(),
        "java_nonnull_annotation": attr.string(),
        "java_implement_android_os_parcelable": attr.string(),
        "java_use_final_for_record": attr.string(),
        "java_gen_interface": attr.string(),
        "java_legacy_records": attr.string(),
        "cpp_namespace": attr.string(),
        "cpp_include_prefix": attr.string(),
        "cpp_base_lib_include_prefix": attr.string(),
        "cpp_optional_template": attr.string(),
        "cpp_optional_header": attr.string(),
        "cpp_extended_record_include_prefix": attr.string(),
        "cpp_use_wide_strings": attr.string(),
        "cpp_legacy_records": attr.string(),
        "c_namespace": attr.string(),
        "c_include_prefix": attr.string(),
        "c_base_lib_include_prefix": attr.string(),
        "c_wrapper_cpp_namespace": attr.string(),
        "c_wrapper_use_dlsym": attr.string(),
        "jni_namespace": attr.string(),
        "jni_include_prefix": attr.string(),
        "jni_include_cpp_prefix": attr.string(),
        "jni_base_lib_include_prefix": attr.string(),
        "jni_use_on_load_initializer": attr.string(),
        "jni_function_prologue_file": attr.string(),
        "objc_type_prefix": attr.string(),
        "objc_include_prefix": attr.string(),
        "objc_base_lib_include_prefix": attr.string(),
        "objc_extended_record_include_prefix": attr.string(),
        "objc_swift_bridging_header": attr.string(),
        "objc_gen_protocol": attr.string(),
        "objc_disable_class_ctor": attr.string(),
        "objc_closed_enums": attr.string(),
        "objc_strict_protocols": attr.string(),
        "objc_legacy_records": attr.string(),
        "objcpp_namespace": attr.string(),
        "objcpp_include_prefix": attr.string(),
        "objcpp_include_cpp_prefix": attr.string(),
        "objcpp_include_objc_prefix": attr.string(),
        "objcpp_function_prologue_file": attr.string(),
        "objcpp_disable_exception_translation": attr.string(),
        "wasm_namespace": attr.string(),
        "wasm_cpp_namespace": attr.string(),
        "wasm_include_prefix": attr.string(),
        "wasm_include_cpp_prefix": attr.string(),
        "wasm_base_lib_include_prefix": attr.string(),
        "wasm_omit_constants": attr.string(),
        "wasm_omit_namespace_alias": attr.string(),
        "ts_module": attr.string(),
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
        "ident_wasm_class": attr.string(),
        "ident_wasm_file": attr.string(),
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
    return result + [file.dirname for file in ctx.files.idl_include_files]

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
        direct = [ctx.file.idl] + ctx.files.srcs + ctx.files.idl_include_files + ([ctx.file.defaults_file] if ctx.file.defaults_file else []) + ([ctx.file.config_file] if ctx.file.config_file else []),
        transitive = transitive,
    )

    declarations_manifest = ctx.actions.declare_file(ctx.label.name + ".declarations.tsv")
    validated_inputs = ctx.actions.declare_file(ctx.label.name + ".djinni.inputs") if ctx.attr.verify else None
    args = ctx.actions.args()
    defaults_file = ctx.file.defaults_file
    config_file = ctx.file.config_file
    config_files = ([defaults_file] if defaults_file else []) + ([config_file] if config_file else [])
    if defaults_file:
        args.add("--defaults")
        args.add(defaults_file.path)
    if config_file:
        args.add("--config")
        args.add(config_file.path)
    args.add("--idl", ctx.file.idl)
    args.add("--skip-generation", "true")
    args.add("--list-declarations", declarations_manifest)
    if validated_inputs:
        args.add("--list-in-files", validated_inputs)
    args.add_all(include_paths, before_each = "--idl-include-path")
    ctx.actions.run(
        executable = ctx.attr.compiler[DefaultInfo].files_to_run,
        arguments = [args],
        inputs = transitive_srcs,
        outputs = [declarations_manifest] + ([validated_inputs] if validated_inputs else []),
        mnemonic = "DjinniDeclarations",
    )
    validation = [validated_inputs] if validated_inputs else []
    validation_files = depset(
        direct = validation,
        transitive = [dep[DjinniInfo].validation_files for dep in ctx.attr.deps],
    )
    return [
        DefaultInfo(files = depset([ctx.file.idl, declarations_manifest] + ctx.files.srcs + ctx.files.idl_include_files + config_files, transitive = [validation_files])),
        DjinniInfo(
            idl = ctx.file.idl,
            transitive_srcs = transitive_srcs,
            idl_include_paths = include_paths,
            validation_files = validation_files,
            declarations_manifest = declarations_manifest,
            project_name = ctx.attr.project_name,
            cpp_namespace = ctx.attr.cpp_namespace,
            cpp_include_prefix = ctx.attr.cpp_include_prefix,
            cpp_base_lib_include_prefix = ctx.attr.cpp_base_lib_include_prefix,
            cpp_optional_template = ctx.attr.cpp_optional_template,
            cpp_optional_header = ctx.attr.cpp_optional_header,
            defaults_file = defaults_file,
            config_file = config_file,
        ),
    ]

djinni_library = rule(
    doc = "Declares one root IDL and its import closure; verifies syntax and types by default.",
    implementation = _djinni_library_impl,
    attrs = {
        "idl": attr.label(allow_single_file = [".djinni"], mandatory = True),
        "srcs": attr.label_list(allow_files = [".djinni", ".yaml", ".yml", ".proto"]),
        "defaults_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "config_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "deps": attr.label_list(providers = [DjinniInfo]),
        "idl_include_paths": attr.string_list(),
        "idl_include_files": attr.label_list(allow_files = True),
        "verify": attr.bool(default = True),
        "compiler": attr.label(default = Label("//src:djinni"), executable = True, cfg = "exec"),
        "project_name": attr.string(default = ""),
        "cpp_namespace": attr.string(default = ""),
        "cpp_include_prefix": attr.string(default = ""),
        "cpp_base_lib_include_prefix": attr.string(default = ""),
        "cpp_optional_template": attr.string(default = ""),
        "cpp_optional_header": attr.string(default = ""),
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
    "java_legacy_records",
    "cpp_legacy_records",
    "objc_legacy_records",
    "c_wrapper_use_dlsym",
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
            cpp_srcs, cpp_hdrs, c_srcs, c_hdrs, java_srcs, jni_srcs, jni_hdrs, objc_srcs,
            objc_hdrs, objcpp_srcs, objcpp_hdrs, wasm_srcs, wasm_hdrs,
            and ts_srcs.
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
        c_srcs = _outs(outs, "c_srcs"),
        c_hdrs = _outs(outs, "c_hdrs"),
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
        **normalized
    )

def _exactly_one_dep(name, deps):
    if len(deps) != 1:
        fail("%s expects exactly one djinni_library dep" % name)

_OUTPUT_KEYS = [
    "cpp_srcs",
    "cpp_hdrs",
    "c_srcs",
    "c_hdrs",
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
]

_COMMON_ATTRS = ["testonly", "tags", "compatible_with", "restricted_to", "target_compatible_with", "features", "deprecation"]

def _cpp_string_option(language_value, idl_value):
    return language_value if language_value else idl_value

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
    languages = [language, "objcpp"] if language == "objc" else [language]
    selected = {key: value for key, value in outs.items() if key.split("_")[0] in languages}
    visibility = options.pop("visibility", ["//visibility:private"])
    djinni_codegen(
        name = name,
        djinni = deps[0],
        outs = selected,
        visibility = visibility,
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

def _generated_tree(name, codegen_name, language, testonly):
    tree_name = codegen_name + "__tree"
    native.filegroup(
        name = tree_name,
        srcs = [":" + codegen_name],
        output_group = language + "_tree",
        testonly = testonly,
    )
    return ":" + tree_name

def _public_objc_headers_impl(ctx):
    source = ctx.file.src
    output = ctx.actions.declare_directory(ctx.label.name + ".headers")
    ctx.actions.run_shell(
        inputs = [source],
        outputs = [output],
        arguments = [source.path, output.path],
        command = "mkdir -p \"$2\"; find \"$1\" -type f -name '*.h' ! -name '*+Private.h' ! -name '*+Impl.h' | while read f; do rel=\"${f#\"$1\"/}\"; mkdir -p \"$2/$(dirname \"$rel\")\"; cp \"$f\" \"$2/$rel\"; done",
    )
    return [DefaultInfo(files = depset([output]))]

djinni_public_objc_headers = rule(
    implementation = _public_objc_headers_impl,
    attrs = {"src": attr.label(allow_single_file = True, mandatory = True)},
)

def _generated_manifest(codegen_name, testonly):
    native.filegroup(
        name = codegen_name + "__manifest",
        srcs = [":" + codegen_name],
        output_group = "manifest",
        testonly = testonly,
    )

def _generated_unity(name, codegen_name, tree, extensions, testonly, output_extension = "cpp"):
    unity_name = name + "__unity"
    native.genrule(
        name = unity_name,
        srcs = [tree, ":" + codegen_name + "__manifest"],
        outs = [name + "__djinni_unity." + output_extension],
        cmd = "tree=$(location %s); sed 's#^'\"$$tree\"'/##' $(location :%s__manifest) | LC_ALL=C sort | while read f; do case \"$$f\" in %s) printf '#include \"%%s\"\\n' \"$$f\";; esac; done > $@" % (tree, codegen_name, "|".join(["*.%s" % ext for ext in extensions])),
        testonly = testonly,
    )
    return ":" + unity_name

def cc_djinni_library(
        name,
        deps,
        outs = None,
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
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["cpp"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "cpp", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "cpp", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["cpp"], kwargs.get("testonly", False))
    cc_library(
        name = name,
        srcs = ([unity] if tree_mode else _outs(outs, "cpp_srcs")) + srcs,
        hdrs = ([tree] if tree_mode else _outs(outs, "cpp_hdrs")) + hdrs,
        additional_compiler_inputs = [tree] if tree_mode else [],
        includes = ([codegen_name + ".cpp"] if tree_mode else []) + includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def c_djinni_library(
        name,
        deps,
        outs = None,
        srcs = [],
        hdrs = [],
        includes = [],
        cc_deps = [],
        implementation_deps = [],
        copts = [],
        linkopts = [],
        alwayslink = 0,
        visibility = None,
        library_kwargs = {},
        **kwargs):
    """Generates Djinni's C-facing wrapper sources in a cc_library."""
    codegen_name = name + "__djinni_c_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["c"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "c", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "c", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["cpp"], kwargs.get("testonly", False))
    cc_library(
        name = name,
        srcs = ([unity] if tree_mode else _outs(outs, "c_srcs")) + srcs,
        hdrs = ([tree] if tree_mode else _outs(outs, "c_hdrs")) + hdrs,
        additional_compiler_inputs = [tree] if tree_mode else [],
        includes = ([codegen_name + ".c"] if tree_mode else []) + includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        implementation_deps = implementation_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def java_djinni_library(
        name,
        deps,
        outs = None,
        java_deps = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates Java from a djinni_library and wraps it in java_library."""
    codegen_name = name + "__djinni_java_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["java"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "java", kwargs)
    if tree_mode:
        native.filegroup(
            name = codegen_name + "__tree",
            srcs = [":" + codegen_name],
            output_group = "java_tree",
            testonly = kwargs.get("testonly", False),
        )
        native.genrule(
            name = name + "__srcjar",
            srcs = [":" + codegen_name + "__tree"],
            outs = [name + "__djinni.srcjar"],
            tools = ["@bazel_tools//tools/zip:zipper"],
            cmd = "zipper=$$(pwd)/$(execpath @bazel_tools//tools/zip:zipper); out=$$(pwd)/$(location %s); cd $(location :%s__tree) && files=$$(find . -name '*.java' -print | LC_ALL=C sort | sed 's#^./##') && $$zipper c $$out $$files" % (name + "__djinni.srcjar", codegen_name),
            testonly = kwargs.get("testonly", False),
        )
    java_library(
        name = name,
        srcs = ([":" + name + "__srcjar"] if tree_mode else _outs(outs, "java_srcs")) + srcs,
        deps = java_deps,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def jni_djinni_library(
        name,
        deps,
        outs = None,
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
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["jni"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "jni", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "jni", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["cpp"], kwargs.get("testonly", False))
    cc_library(
        name = name,
        srcs = ([unity] if tree_mode else _outs(outs, "jni_srcs")) + srcs,
        hdrs = ([tree] if tree_mode else _outs(outs, "jni_hdrs")) + hdrs,
        additional_compiler_inputs = [tree] if tree_mode else [],
        includes = ([codegen_name + ".jni"] if tree_mode else []) + includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def objc_djinni_library(
        name,
        deps,
        outs = None,
        srcs = [],
        hdrs = [],
        includes = [],
        objc_deps = [],
        copts = [],
        linkopts = [],
        sdk_frameworks = [],
        weak_sdk_frameworks = [],
        alwayslink = 0,
        visibility = None,
        library_kwargs = {},
        **kwargs):
    """Generates Objective-C and Objective-C++ sources in an objc_library."""
    codegen_name = name + "__djinni_objc_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["objc"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "objc", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "objc", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["m", "mm"], kwargs.get("testonly", False), "mm")
    objc_library(
        name = name,
        srcs = ([unity] if tree_mode else _outs(outs, "objc_srcs") + _outs(outs, "objcpp_srcs")) + srcs,
        hdrs = ([tree] if tree_mode else _outs(outs, "objc_hdrs") + _outs(outs, "objcpp_hdrs")) + hdrs,
        includes = ([codegen_name + ".objc"] if tree_mode else []) + includes,
        copts = copts,
        linkopts = linkopts,
        deps = objc_deps,
        sdk_frameworks = sdk_frameworks,
        weak_sdk_frameworks = weak_sdk_frameworks,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def wasm_djinni_cc_library(
        name,
        deps,
        outs = None,
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
    """Generates WASM bridge sources in a cc_library for caller-owned linking."""
    codegen_name = name + "__djinni_wasm_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["wasm"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "wasm", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "wasm", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["cpp"], kwargs.get("testonly", False))
    cc_library(
        name = name,
        srcs = ([unity] if tree_mode else _outs(outs, "wasm_srcs")) + srcs,
        hdrs = ([tree] if tree_mode else _outs(outs, "wasm_hdrs")) + hdrs,
        additional_compiler_inputs = [tree] if tree_mode else [],
        includes = ([codegen_name + ".wasm"] if tree_mode else []) + includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def ts_djinni_filegroup(name, deps, outs = None, srcs = [], visibility = None, **kwargs):
    """Generates TypeScript and exposes it as a filegroup."""
    codegen_name = name + "__djinni_ts_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["ts"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "ts", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "ts", kwargs.get("testonly", False))
        native.genrule(
            name = name + "__source",
            srcs = [tree],
            outs = [name + ".ts"],
            cmd = "cp $(location %s)/%s.ts $@" % (tree, kwargs.get("ts_module", "module")),
            testonly = kwargs.get("testonly", False),
        )
    native.filegroup(
        name = name,
        srcs = ([":" + name + "__source"] if tree_mode else [":" + codegen_name]) + srcs,
        output_group = None if tree_mode else "ts_srcs",
        visibility = visibility or ["//visibility:private"],
        **common
    )

def wasm_djinni_cc_binary(
        name,
        deps,
        outs = None,
        cc_deps = [],
        copts = [],
        linkopts = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates WASM C++ bridge sources from a djinni_library and wraps them in cc_binary."""
    codegen_name = name + "__djinni_wasm_codegen"
    tree_mode = outs == None
    if tree_mode:
        kwargs["tree_languages"] = ["wasm"]
        outs = {}
    common = _language_codegen(codegen_name, deps, outs, "wasm", kwargs)
    if tree_mode:
        tree = _generated_tree(name, codegen_name, "wasm", kwargs.get("testonly", False))
        _generated_manifest(codegen_name, kwargs.get("testonly", False))
        unity = _generated_unity(name, codegen_name, tree, ["cpp"], kwargs.get("testonly", False))
        cc_library(
            name = name + "__generated",
            srcs = [unity],
            hdrs = [tree],
            additional_compiler_inputs = [tree],
            deps = cc_deps,
            copts = copts,
            linkopts = linkopts,
            testonly = kwargs.get("testonly", False),
        )
    cc_binary(
        name = name,
        srcs = ([] if tree_mode else _outs(outs, "wasm_srcs") + _outs(outs, "wasm_hdrs")) + srcs,
        copts = copts,
        linkopts = linkopts,
        deps = ([":" + name + "__generated"] if tree_mode else cc_deps),
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )
