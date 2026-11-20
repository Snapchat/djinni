"""Bazel rules for running Djinni code generation."""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "objc_library")
load("@rules_java//java:defs.bzl", "java_library")

DjinniInfo = provider(
    doc = "Djinni IDL provider.",
    fields = {
        "idl": "Root .djinni file.",
        "transitive_srcs": "Root IDL and imported IDL/config sources from the transitive dependency closure.",
        "idl_include_paths": "Depset of execution-root include paths.",
        "types": "Map from declaration category (enums, records, interfaces) to names in this schema's resolved import closure.",
        "types_provided": "Whether the caller supplied a complete declaration list, including an empty list.",
        "declarations_manifest": "Resolved internal declarations produced by Djinni.",
        "output_manifest": "Per-language generated file inventory produced from the IDL and its configuration.",
        "project_name": "Consumer project name for language-specific defaults.",
        "cpp_namespace": "Default C++ namespace.",
        "cpp_include_prefix": "Default generated C++ include prefix.",
        "cpp_base_lib_include_prefix": "Default C++ runtime include prefix.",
        "cpp_optional_template": "Default C++ optional template.",
        "cpp_optional_header": "Default C++ optional header.",
        "defaults_file": "Organization-wide default generation configuration YAML.",
        "config_file": "IDL-specific generation configuration YAML, applied after defaults.",
        "yaml_tree": "Generated external-type YAML files for this IDL and its imported declarations.",
        "yaml_files": "Depset of generated YAML tree artifacts in the import closure.",
        "input_manifest": "Manifest of IDL files read by this declaration and its imports.",
    },
)

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
        manifests = [djinni.declarations_manifest]
        if djinni.input_manifest:
            manifests.append(djinni.input_manifest)
        input_depsets = [djinni.transitive_srcs, djinni.yaml_files, depset(manifests)]
        idl_include_paths = djinni.idl_include_paths.to_list() + _include_paths(ctx)
    else:
        idl = ctx.file.idl
        input_depsets = []
        idl_include_paths = _include_paths(ctx)
        if not idl:
            fail("djinni_codegen requires either idl or djinni")
    if ctx.attr.djinni and ctx.file.idl:
        fail("Specify only one of djinni or idl")

    all_outputs = []
    tree_outputs = {}
    for language in ctx.attr.tree_languages:
        if language not in ["cpp", "c", "java", "jni", "objc", "wasm", "ts", "swift", "swiftxx"]:
            fail("Unsupported tree output language: %s" % language)
        if language in tree_outputs:
            fail("Duplicate tree output language: %s" % language)
        tree_outputs[language] = ctx.actions.declare_directory(ctx.label.name + "." + language)
        all_outputs.append(tree_outputs[language])
    if not tree_outputs:
        fail("djinni_codegen requires at least one tree language")
    owned_manifest = ctx.actions.declare_file(ctx.label.name + ".outputs_by_declaration.tsv")
    all_outputs.append(owned_manifest)
    manifest = ctx.actions.declare_file(ctx.label.name + ".outputs")
    all_outputs.append(manifest)

    args = ctx.actions.args()
    defaults_file = djinni.defaults_file if ctx.attr.djinni else ctx.file.defaults_file
    config_file = djinni.config_file if ctx.attr.djinni else ctx.file.config_file
    config_files = ([defaults_file] if defaults_file else []) + ([config_file] if config_file else [])

    java_package_path = ctx.attr.java_package.replace(".", "/")
    java_dir = (tree_outputs["java"].path + "/" + java_package_path if java_package_path else tree_outputs["java"].path) if "java" in tree_outputs else None
    cpp_prefix = ctx.attr.cpp_include_prefix or (ctx.attr.djinni[DjinniInfo].cpp_include_prefix if ctx.attr.djinni else "")
    cpp_tree_dir = tree_outputs["cpp"].path + "/" + cpp_prefix.rstrip("/") if "cpp" in tree_outputs and cpp_prefix else (tree_outputs["cpp"].path if "cpp" in tree_outputs else None)
    cpp_src_dir = cpp_tree_dir
    cpp_hdr_dir = cpp_tree_dir
    c_prefix = ctx.attr.c_include_prefix.rstrip("/")
    c_tree_dir = tree_outputs["c"].path + "/" + c_prefix if "c" in tree_outputs and c_prefix else (tree_outputs["c"].path if "c" in tree_outputs else None)
    c_src_dir = tree_outputs["c"].path if "c" in tree_outputs else None
    c_hdr_dir = c_tree_dir
    jni_prefix = ctx.attr.jni_include_prefix.rstrip("/")
    jni_tree_dir = tree_outputs["jni"].path + "/" + jni_prefix if "jni" in tree_outputs and jni_prefix else (tree_outputs["jni"].path if "jni" in tree_outputs else None)
    jni_src_dir = jni_tree_dir
    jni_hdr_dir = jni_tree_dir
    objc_prefix = ctx.attr.objc_include_prefix.rstrip("/")
    objc_tree_dir = tree_outputs["objc"].path + "/" + objc_prefix if "objc" in tree_outputs and objc_prefix else (tree_outputs["objc"].path if "objc" in tree_outputs else None)
    objc_dir = objc_tree_dir
    objcpp_dir = objc_tree_dir
    wasm_prefix = ctx.attr.wasm_include_prefix.rstrip("/")
    wasm_tree_dir = tree_outputs["wasm"].path + "/" + wasm_prefix if "wasm" in tree_outputs and wasm_prefix else (tree_outputs["wasm"].path if "wasm" in tree_outputs else None)
    wasm_dir = wasm_tree_dir
    ts_dir = tree_outputs["ts"].path if "ts" in tree_outputs else None
    swift_dir = tree_outputs["swift"].path if "swift" in tree_outputs else None
    swiftxx_dir = tree_outputs["swiftxx"].path if "swiftxx" in tree_outputs else None

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
    if swift_dir:
        args.add("--swift-out")
        args.add(swift_dir)
    if swiftxx_dir:
        args.add("--swiftxx-out")
        args.add(swiftxx_dir)
    if not config_file:
        _add_string_arg(args, "--ts-module", ctx.attr.ts_module)
        _add_string_arg(args, "--swift-module", ctx.attr.swift_module)
        if ctx.attr.swift_private_in_same_module:
            args.add("--swift-private-in-same-module")
        _add_string_arg(args, "--swiftxx-include-prefix", ctx.attr.swiftxx_include_prefix)
        _add_string_arg(args, "--swiftxx-include-cpp-prefix", ctx.attr.swiftxx_include_cpp_prefix)
        _add_string_arg(args, "--swiftxx-base-lib-include-prefix", ctx.attr.swiftxx_base_lib_include_prefix)

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
        _add_ident_arg(args, "--ident-swiftxx-class", ctx.attr.ident_swiftxx_class)
        _add_ident_arg(args, "--ident-swiftxx-file", ctx.attr.ident_swiftxx_file)

    if defaults_file:
        args.add("--defaults")
        args.add(defaults_file.path)
    if config_file:
        args.add("--config")
        args.add(config_file.path)

    args.add("--idl")
    args.add(idl.path)
    args.add("--list-out-files", manifest.path)
    if owned_manifest:
        args.add("--list-out-files-with-owner")
        args.add(owned_manifest.path)
    if ctx.attr.djinni:
        args.add("--verify-declarations")
        args.add(djinni.declarations_manifest.path)
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
            outputs_by_declaration = depset([owned_manifest] if owned_manifest else []),
            cpp_tree = depset([tree_outputs["cpp"]] if "cpp" in tree_outputs else []),
            java_tree = depset([tree_outputs["java"]] if "java" in tree_outputs else []),
            c_tree = depset([tree_outputs["c"]] if "c" in tree_outputs else []),
            jni_tree = depset([tree_outputs["jni"]] if "jni" in tree_outputs else []),
            objc_tree = depset([tree_outputs["objc"]] if "objc" in tree_outputs else []),
            wasm_tree = depset([tree_outputs["wasm"]] if "wasm" in tree_outputs else []),
            ts_tree = depset([tree_outputs["ts"]] if "ts" in tree_outputs else []),
            swift_tree = depset([tree_outputs["swift"]] if "swift" in tree_outputs else []),
            swiftxx_tree = depset([tree_outputs["swiftxx"]] if "swiftxx" in tree_outputs else []),
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
        "swift_module": attr.string(),
        "swift_private_in_same_module": attr.bool(),
        "swiftxx_include_prefix": attr.string(),
        "swiftxx_include_cpp_prefix": attr.string(),
        "swiftxx_base_lib_include_prefix": attr.string(),
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
        "ident_swiftxx_class": attr.string(),
        "ident_swiftxx_file": attr.string(),
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
    for file in ctx.files.idl_include_files:
        owner = file.owner
        if owner and owner.workspace_root:
            result.append(owner.workspace_root)
        result.append(file.dirname)
    return result

_TYPE_CATEGORIES = ["enums", "records", "interfaces"]
_TYPE_FLAGS = {
    "enums": "--expected-enum",
    "records": "--expected-record",
    "interfaces": "--expected-interface",
}

def _normalized_types(types):
    return {category: types.get(category, []) for category in _TYPE_CATEGORIES}

def _validate_type_lists(ctx):
    for category in ctx.attr.types:
        if category not in _TYPE_CATEGORIES:
            fail("Unknown type category %r; expected one of: %s" % (category, ", ".join(_TYPE_CATEGORIES)))

    owner_by_name = {}
    for category in _TYPE_CATEGORIES:
        for name in ctx.attr.types.get(category, []):
            if name in owner_by_name:
                fail("Declaration %s appears more than once (as %s and %s)" % (name, owner_by_name[name], category))
            owner_by_name[name] = category

def _djinni_library_impl(ctx):
    _validate_type_lists(ctx)
    types = _normalized_types(ctx.attr.types)
    types_provided = ctx.attr.types_provided
    transitive = [
        dep[DjinniInfo].transitive_srcs
        for dep in ctx.attr.deps
    ]
    dependency_yaml_trees = [dep[DjinniInfo].yaml_tree for dep in ctx.attr.deps]
    yaml_files = depset(
        direct = dependency_yaml_trees,
        transitive = [dep[DjinniInfo].yaml_files for dep in ctx.attr.deps],
    )
    include_paths = depset(
        direct = _include_paths(ctx) + [tree.path for tree in dependency_yaml_trees],
        transitive = [dep[DjinniInfo].idl_include_paths for dep in ctx.attr.deps],
    )

    yaml_tree = ctx.actions.declare_directory(ctx.label.name + ".yaml")
    transitive_srcs = depset(
        direct = [ctx.file.idl] + ctx.files.srcs + ctx.files.idl_include_files + ([ctx.file.defaults_file] if ctx.file.defaults_file else []) + ([ctx.file.config_file] if ctx.file.config_file else []),
        transitive = transitive,
    )

    declarations_manifest = ctx.actions.declare_file(ctx.label.name + ".declarations.tsv")
    output_manifest = ctx.actions.declare_file(ctx.label.name + ".outputs")
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
    args.add("--yaml-out", yaml_tree.path + "/" + ctx.attr.project_name)
    args.add("--idl", ctx.file.idl)
    args.add("--list-declarations", declarations_manifest)
    if types_provided:
        args.add("--verify-type-lists", "true")
        for category in _TYPE_CATEGORIES:
            args.add_all(types[category], before_each = _TYPE_FLAGS[category])
    if validated_inputs:
        args.add("--list-in-files", validated_inputs)
    args.add_all(include_paths.to_list(), before_each = "--idl-include-path")
    ctx.actions.run(
        executable = ctx.attr.compiler[DefaultInfo].files_to_run,
        arguments = [args],
        inputs = depset(direct = [ctx.file.idl] + ctx.files.srcs + ctx.files.idl_include_files + config_files, transitive = transitive + [yaml_files]),
        outputs = [declarations_manifest, yaml_tree] + ([validated_inputs] if validated_inputs else []),
        mnemonic = "DjinniDeclarations",
    )

    # Discover generated filenames once from the root IDL and its merged
    # configuration. Keep each backend under a stable prefix so downstream
    # language rules can select their entries from the shared manifest.
    output_args = ctx.actions.args()
    if defaults_file:
        output_args.add("--defaults", defaults_file.path)
    if config_file:
        output_args.add("--config", config_file.path)
    output_args.add("--cpp-out", "__djinni_outputs/cpp_src")
    output_args.add("--cpp-header-out", "__djinni_outputs/cpp_hdr")
    output_args.add("--c-out", "__djinni_outputs/c_src")
    output_args.add("--c-header-out", "__djinni_outputs/c_hdr")
    output_args.add("--java-out", "__djinni_outputs/java")
    output_args.add("--jni-out", "__djinni_outputs/jni_src")
    output_args.add("--jni-header-out", "__djinni_outputs/jni_hdr")
    output_args.add("--objc-out", "__djinni_outputs/objc")
    output_args.add("--objcpp-out", "__djinni_outputs/objcpp")
    output_args.add("--wasm-out", "__djinni_outputs/wasm")
    output_args.add("--ts-out", "__djinni_outputs/ts")
    output_args.add("--swift-out", "__djinni_outputs/swift")
    output_args.add("--swiftxx-out", "__djinni_outputs/swiftxx")
    output_args.add("--valdi-out", "__djinni_outputs/valdi")
    output_args.add("--valdi-ts-out", "__djinni_outputs/valdi_ts")
    output_args.add("--skip-generation", "true")
    output_args.add("--list-out-files", output_manifest)
    output_args.add("--idl", ctx.file.idl)
    output_args.add_all(include_paths.to_list(), before_each = "--idl-include-path")
    ctx.actions.run(
        executable = ctx.attr.compiler[DefaultInfo].files_to_run,
        arguments = [output_args],
        inputs = depset(direct = [ctx.file.idl] + ctx.files.srcs + ctx.files.idl_include_files + config_files, transitive = transitive + [yaml_files]),
        outputs = [output_manifest],
        mnemonic = "DjinniOutputInventory",
        progress_message = "Listing Djinni outputs for %s" % ctx.label,
    )
    return [
        DefaultInfo(files = depset([ctx.file.idl, declarations_manifest, output_manifest, yaml_tree] + ([validated_inputs] if validated_inputs else []) + ctx.files.srcs + ctx.files.idl_include_files + config_files, transitive = [depset(transitive = transitive)])),
        OutputGroupInfo(yaml_tree = depset([yaml_tree])),
        DjinniInfo(
            idl = ctx.file.idl,
            transitive_srcs = transitive_srcs,
            idl_include_paths = include_paths,
            types = types,
            types_provided = types_provided,
            declarations_manifest = declarations_manifest,
            output_manifest = output_manifest,
            project_name = ctx.attr.project_name,
            cpp_namespace = ctx.attr.cpp_namespace,
            cpp_include_prefix = ctx.attr.cpp_include_prefix,
            cpp_base_lib_include_prefix = ctx.attr.cpp_base_lib_include_prefix,
            cpp_optional_template = ctx.attr.cpp_optional_template,
            cpp_optional_header = ctx.attr.cpp_optional_header,
            defaults_file = defaults_file,
            config_file = config_file,
            yaml_tree = yaml_tree,
            yaml_files = depset(direct = [yaml_tree], transitive = [yaml_files]),
            input_manifest = validated_inputs,
        ),
    ]

_djinni_library_rule = rule(
    doc = "Declares one root IDL and its import closure; verifies syntax and types by default.",
    implementation = _djinni_library_impl,
    attrs = {
        "idl": attr.label(allow_single_file = [".djinni"], mandatory = True),
        "srcs": attr.label_list(allow_files = [".djinni", ".yaml", ".yml", ".proto"]),
        "defaults_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "config_file": attr.label(allow_single_file = [".yaml", ".yml"]),
        "deps": attr.label_list(providers = [DjinniInfo]),
        "types": attr.string_list_dict(),
        "types_provided": attr.bool(default = False),
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

def djinni_library(name, types = None, **kwargs):
    """Declares one root IDL and whether its complete declaration list is supplied."""
    _djinni_library_rule(
        name = name,
        types = {} if types == None else types,
        types_provided = types != None,
        **kwargs
    )

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

def djinni_codegen(name, **kwargs):
    """Runs Djinni and exposes generated language trees through output groups.

    Args:
        name: Rule name.
        **kwargs: Djinni options, input labels, and required tree_languages.
    """
    if not kwargs.get("tree_languages"):
        fail("djinni_codegen requires tree_languages")
    normalized = _normalize_kwargs(kwargs)
    _djinni_codegen(name = name, **normalized)

def _exactly_one_dep(name, deps):
    if len(deps) != 1:
        fail("%s expects exactly one djinni_library dep" % name)

_COMMON_ATTRS = ["testonly", "tags", "compatible_with", "restricted_to", "target_compatible_with", "features", "deprecation"]

def _cpp_string_option(language_value, idl_value):
    return language_value if language_value else idl_value

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

def _objc_headers_tree_impl(ctx):
    source = ctx.file.src
    output = ctx.actions.declare_directory(ctx.label.name + ".headers")
    ctx.actions.run_shell(
        inputs = [source],
        outputs = [output],
        arguments = [source.path, output.path, ctx.attr.header_mode],
        command = """
set -euo pipefail
source="$1"
output="$2"
header_mode="$3"
mkdir -p "$output"
if [ "$header_mode" = "private" ]; then
    pattern='*+Private.h'
else
    pattern='*.h'
fi
find -L "$source" -type f -name "$pattern" | while IFS= read -r file; do
    relative="${file#"$source"/}"
    mkdir -p "$output/$(dirname "$relative")"
    cp "$file" "$output/$relative"
done
""",
    )
    return [DefaultInfo(files = depset([output]))]

_objc_headers_tree = rule(
    implementation = _objc_headers_tree_impl,
    attrs = {
        "header_mode": attr.string(default = "all", values = ["all", "private"]),
        "src": attr.label(allow_single_file = True, mandatory = True),
    },
)

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

def _source_slots(language, djinni):
    slots = []
    enums = djinni.types["enums"]
    records = djinni.types["records"]
    interfaces = djinni.types["interfaces"]
    if language == "cpp":
        names = records + interfaces
        slots += [struct(backend = "cpp", name = name, extension = ".cpp") for name in names]
    elif language == "c" or language == "wasm":
        names = enums + records + interfaces
        slots += [struct(backend = language, name = name, extension = ".cpp") for name in names]
    elif language == "jni":
        names = records + interfaces
        slots += [struct(backend = "jni", name = name, extension = ".cpp") for name in names]
        slots.append(struct(backend = "jni", name = "__module__", extension = ".cpp"))
    elif language == "objc":
        names = records + interfaces
        for backend in ["objc", "objcpp"]:
            slots += [struct(backend = backend, name = name, extension = ".mm") for name in names]
    else:
        fail("Declaration source extraction is unsupported for %s" % language)
    return slots

def _typed_source_files_impl(ctx):
    djinni = ctx.attr.djinni[DjinniInfo]
    if not djinni.types_provided:
        output_extension = "mm" if ctx.attr.language == "objc" else "cpp"
        output = ctx.actions.declare_file(ctx.label.name + "__unity." + output_extension)
        command = """set -euo pipefail
tree="$1"
manifest="$2"
output="$3"
language="$4"
while IFS="$(printf '\\t')" read -r row_backend row_role row_kind row_name file; do
    backend_matches=false
    if [ "$language" = "objc" ]; then
        if [ "$row_backend" = "objc" ] || [ "$row_backend" = "objcpp" ]; then
            backend_matches=true
        fi
    elif [ "$row_backend" = "$language" ]; then
        backend_matches=true
    fi
    if [ "$row_role" = "source" ] && [ "$backend_matches" = "true" ]; then
        case "$file" in
            "$tree"/*) printf '%s\\n' "$file" ;;
        esac
    fi
done < "$manifest" | LC_ALL=C sort | while IFS= read -r file; do
    relative="${file#"$tree"/}"
    printf '#line 1 \"%s\"\\n' "$relative"
    cat "$file"
    printf '\\n'
done > "$output"
"""
        ctx.actions.run_shell(
            inputs = [ctx.file.source_tree, ctx.file.owned_manifest],
            outputs = [output],
            arguments = [ctx.file.source_tree.path, ctx.file.owned_manifest.path, output.path, ctx.attr.language],
            command = command,
            mnemonic = "DjinniUnityFallback",
        )
        return [DefaultInfo(files = depset([output]))]

    outputs = []
    for slot in _source_slots(ctx.attr.language, djinni):
        output = ctx.actions.declare_file(
            ctx.label.name + "__typed_sources/" + slot.name + "__" + slot.backend + slot.extension,
        )
        ctx.actions.run_shell(
            inputs = [ctx.file.source_tree, ctx.file.owned_manifest],
            outputs = [output],
            arguments = [
                ctx.file.source_tree.path,
                ctx.file.owned_manifest.path,
                output.path,
                slot.backend,
                slot.name,
            ],
            command = """set -euo pipefail
tree="$1"
manifest="$2"
output="$3"
backend="$4"
declaration="$5"
found=""
while IFS="$(printf '\\t')" read -r row_backend row_role row_kind row_name file; do
    if [ "$row_backend" = "$backend" ] && [ "$row_role" = "source" ] && [ "$row_name" = "$declaration" ]; then
        if [ -n "$found" ]; then
            echo "More than one $backend source file found for $declaration" >&2
            exit 1
        fi
        found="$file"
    fi
done < "$manifest"
if [ -n "$found" ]; then
    case "$found" in
        "$tree"/*) cp "$found" "$output" ;;
        *) echo "Owned source path is outside its generated tree: $found" >&2; exit 1 ;;
    esac
else
    : > "$output"
fi
""",
            mnemonic = "DjinniTypedSource",
            progress_message = "Selecting %s source for %s" % (slot.backend, slot.name),
        )
        outputs.append(output)
    return [DefaultInfo(files = depset(outputs))]

_typed_source_files = rule(
    implementation = _typed_source_files_impl,
    attrs = {
        "djinni": attr.label(mandatory = True, providers = [DjinniInfo]),
        "language": attr.string(mandatory = True, values = ["cpp", "c", "jni", "objc", "wasm"]),
        "owned_manifest": attr.label(allow_single_file = True, mandatory = True),
        "source_tree": attr.label(allow_single_file = True, mandatory = True),
    },
)

def _owned_headers_tree_impl(ctx):
    output = ctx.actions.declare_directory(ctx.label.name + ".headers")
    ctx.actions.run_shell(
        inputs = [ctx.file.source_tree, ctx.file.owned_manifest],
        outputs = [output],
        arguments = [ctx.file.source_tree.path, ctx.file.owned_manifest.path, output.path],
        command = """set -euo pipefail
tree="$1"
manifest="$2"
output="$3"
mkdir -p "$output"
while IFS="$(printf '\\t')" read -r backend row_role row_kind row_name file; do
    if [ "$row_role" = "header" ]; then
        case "$file" in
            "$tree"/*)
                relative="${file#"$tree"/}"
                mkdir -p "$output/$(dirname "$relative")"
                cp "$file" "$output/$relative"
                ;;
        esac
    fi
done < "$manifest"
""",
        mnemonic = "DjinniGeneratedHeaders",
    )
    return [DefaultInfo(files = depset([output]))]

_owned_headers_tree = rule(
    implementation = _owned_headers_tree_impl,
    attrs = {
        "owned_manifest": attr.label(allow_single_file = True, mandatory = True),
        "source_tree": attr.label(allow_single_file = True, mandatory = True),
    },
)

def _generated_owned_manifest(name, codegen_name, testonly):
    native.filegroup(
        name = name,
        srcs = [":" + codegen_name],
        output_group = "outputs_by_declaration",
        testonly = testonly,
    )

def _generated_sources(name, tree, owned_manifest, language, djinni, testonly):
    sources_name = name + "__generated_sources"
    _typed_source_files(
        name = sources_name,
        djinni = djinni,
        language = language,
        owned_manifest = owned_manifest,
        source_tree = tree,
        testonly = testonly,
    )
    return ":" + sources_name

def _generated_owned_headers(name, tree, owned_manifest, testonly):
    headers_name = name + "__generated_headers"
    _owned_headers_tree(
        name = headers_name,
        owned_manifest = owned_manifest,
        source_tree = tree,
        testonly = testonly,
    )
    return ":" + headers_name, headers_name + ".headers"

def _language_tree_codegen(name, deps, language, kwargs):
    _exactly_one_dep(name, deps)
    options = dict(kwargs)
    common = {}
    for attr_name in _COMMON_ATTRS:
        if attr_name in options:
            common[attr_name] = options.pop(attr_name)
    visibility = options.pop("visibility", ["//visibility:private"])
    options["tree_languages"] = [language]
    djinni_codegen(
        name = name,
        djinni = deps[0],
        visibility = visibility,
        **dict(options, **common)
    )
    return common

def cc_djinni_library(
        name,
        deps,
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
    """Generates C++ from declared types, or a unity fallback when types are absent."""
    codegen_name = name + "__djinni_cpp_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "cpp", kwargs)
    tree = _generated_tree(name, codegen_name, "cpp", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "cpp", deps[0], testonly)
    generated_headers, generated_include_root = _generated_owned_headers(name, tree, ":" + owned_manifest, testonly)
    cc_library(
        name = name,
        srcs = [generated_sources] + srcs,
        hdrs = [generated_headers] + hdrs,
        includes = [generated_include_root] + includes,
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
    """Generates C-facing wrapper sources from declared types, or a unity fallback."""
    codegen_name = name + "__djinni_c_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "c", kwargs)
    tree = _generated_tree(name, codegen_name, "c", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "c", deps[0], testonly)
    generated_headers, generated_include_root = _generated_owned_headers(name, tree, ":" + owned_manifest, testonly)
    cc_library(
        name = name,
        srcs = [generated_sources] + srcs,
        hdrs = [generated_headers] + hdrs,
        includes = [generated_include_root] + includes,
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
        java_deps = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates Java from a djinni_library and wraps it in java_library."""
    codegen_name = name + "__djinni_java_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "java", kwargs)
    tree = _generated_tree(name, codegen_name, "java", testonly)
    _generated_manifest(codegen_name, testonly)
    native.genrule(
        name = name + "__srcjar",
        srcs = [tree, ":" + codegen_name + "__manifest"],
        outs = [name + "__djinni.srcjar"],
        tools = ["@bazel_tools//tools/zip:zipper"],
        cmd = "tree=$(location %s); zipper=$$(pwd)/$(execpath @bazel_tools//tools/zip:zipper); out=$$(pwd)/$(location %s); files=$$(sed 's#^'\"$$tree\"'/##' $(location :%s__manifest) | LC_ALL=C sort); cd $$tree && $$zipper c $$out $$files" % (codegen_name + "__tree", name + "__djinni.srcjar", codegen_name),
        testonly = testonly,
    )
    java_library(
        name = name,
        srcs = [":" + name + "__srcjar"] + srcs,
        deps = java_deps,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def jni_djinni_library(
        name,
        deps,
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
    """Generates JNI C++ from declared types, or a unity fallback when types are absent."""
    codegen_name = name + "__djinni_jni_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "jni", kwargs)
    tree = _generated_tree(name, codegen_name, "jni", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "jni", deps[0], testonly)
    generated_headers, generated_include_root = _generated_owned_headers(name, tree, ":" + owned_manifest, testonly)
    cc_library(
        name = name,
        srcs = [generated_sources] + srcs,
        hdrs = [generated_headers] + hdrs,
        includes = [generated_include_root] + includes,
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
        generated_header_mode = "all",
        **kwargs):
    """Generates Objective-C sources per declared type, or unity fallback without type metadata."""
    codegen_name = name + "__djinni_objc_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "objc", kwargs)
    tree = _generated_tree(name, codegen_name, "objc", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "objc", deps[0], testonly)
    generated_headers = name + "__objc_headers"
    _objc_headers_tree(
        name = generated_headers,
        src = tree,
        header_mode = generated_header_mode,
        testonly = testonly,
    )
    generated_includes = [generated_headers + ".headers"]
    objc_prefix = kwargs.get("objc_include_prefix", "").rstrip("/")
    if objc_prefix:
        generated_includes.append(generated_headers + ".headers/" + objc_prefix)
    objc_library(
        name = name,
        srcs = [generated_sources] + srcs,
        hdrs = [":" + generated_headers] + hdrs,
        includes = generated_includes + includes,
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
        srcs = [],
        hdrs = [],
        includes = [],
        cc_deps = [],
        copts = [],
        linkopts = [],
        alwayslink = 0,
        visibility = None,
        library_kwargs = {},
        additional_compiler_inputs = [],
        **kwargs):
    """Generates WASM bridge sources per declared type, or unity fallback without type metadata."""
    codegen_name = name + "__djinni_wasm_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "wasm", kwargs)
    tree = _generated_tree(name, codegen_name, "wasm", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "wasm", deps[0], testonly)
    generated_headers, generated_include_root = _generated_owned_headers(name, tree, ":" + owned_manifest, testonly)
    cc_library(
        name = name,
        srcs = [generated_sources] + srcs,
        hdrs = [generated_headers] + hdrs,
        additional_compiler_inputs = additional_compiler_inputs,
        includes = [generated_include_root] + includes,
        copts = copts,
        linkopts = linkopts,
        deps = cc_deps,
        alwayslink = alwayslink,
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )

def ts_djinni_filegroup(name, deps, srcs = [], visibility = None, **kwargs):
    """Generates TypeScript and exposes it as a filegroup."""
    codegen_name = name + "__djinni_ts_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "ts", kwargs)
    tree = _generated_tree(name, codegen_name, "ts", testonly)
    _generated_manifest(codegen_name, testonly)
    native.genrule(
        name = name + "__source",
        srcs = [tree, ":" + codegen_name + "__manifest"],
        outs = [name + ".ts"],
        cmd = "tree=$(location %s); file=$$(sed 's#^'\"$$tree\"'/##' $(location :%s__manifest)); cp \"$$tree/$$file\" $@" % (codegen_name + "__tree", codegen_name),
        testonly = testonly,
    )
    native.filegroup(
        name = name,
        srcs = [":" + name + "__source"] + srcs,
        visibility = visibility or ["//visibility:private"],
        **common
    )

def wasm_djinni_cc_binary(
        name,
        deps,
        cc_deps = [],
        copts = [],
        linkopts = [],
        visibility = None,
        srcs = [],
        library_kwargs = {},
        **kwargs):
    """Generates WASM C++ bridge sources from a djinni_library and wraps them in cc_binary."""
    codegen_name = name + "__djinni_wasm_codegen"
    testonly = kwargs.get("testonly", False)
    common = _language_tree_codegen(codegen_name, deps, "wasm", kwargs)
    tree = _generated_tree(name, codegen_name, "wasm", testonly)
    owned_manifest = name + "__owned_manifest"
    _generated_owned_manifest(owned_manifest, codegen_name, testonly)
    generated_sources = _generated_sources(name, tree, ":" + owned_manifest, "wasm", deps[0], testonly)
    generated_headers, generated_include_root = _generated_owned_headers(name, tree, ":" + owned_manifest, testonly)
    cc_library(
        name = name + "__generated",
        srcs = [generated_sources],
        hdrs = [generated_headers],
        includes = [generated_include_root],
        deps = cc_deps,
        copts = copts,
        linkopts = linkopts,
        testonly = testonly,
    )
    cc_binary(
        name = name,
        srcs = srcs,
        copts = copts,
        linkopts = linkopts,
        deps = [":" + name + "__generated"],
        visibility = visibility,
        **_consumer_attrs(library_kwargs, common)
    )
