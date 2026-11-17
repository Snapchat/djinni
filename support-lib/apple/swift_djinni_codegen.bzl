"""Bazel macro for compiling Djinni-generated Swift and Swift/C++ bridges."""

load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@djinni_rules//bzl:djinni_codegen.bzl", "DjinniInfo", "djinni_codegen")

_DJINNI_SWIFT_SUPPORT = Label("//:djinni-support-swift")
_DJINNI_SWIFTXX_SUPPORT = Label("//:djinni-support-swiftxx")
_DJINNI_SWIFTXX_SUPPORT_CORE = Label("@djinni_rules//support-lib:djinni-support-swiftxx-core")
_COMMON_ATTRS = ["testonly", "tags", "compatible_with", "restricted_to", "target_compatible_with", "features", "deprecation"]

def _declaration_names(djinni):
    return djinni.enums + djinni.records + djinni.interfaces

def _typed_swift_outputs_impl(ctx):
    djinni = ctx.attr.djinni[DjinniInfo]
    names = _declaration_names(djinni)
    if not names:
        fail("swift_djinni_library requires declaration metadata on its djinni_library dependency")

    public_swift = []
    private_swift = []
    swiftxx_sources = []
    for name in names:
        for variant, backend, bucket in [
            ("public", "swift", public_swift),
            ("private", "swift", private_swift),
            ("source", "swiftxx", swiftxx_sources),
        ]:
            extension = ".cpp" if backend == "swiftxx" else ".swift"
            output = ctx.actions.declare_file(ctx.label.name + "__typed/" + name + "__" + variant + extension)
            ctx.actions.run_shell(
                inputs = [ctx.file.swift_tree, ctx.file.swiftxx_tree, ctx.file.owned_manifest],
                outputs = [output],
                arguments = [
                    ctx.file.swift_tree.path,
                    ctx.file.swiftxx_tree.path,
                    ctx.file.owned_manifest.path,
                    output.path,
                    backend,
                    name,
                    variant,
                ],
                command = """set -euo pipefail
swift_tree="$1"
swiftxx_tree="$2"
manifest="$3"
output="$4"
backend="$5"
declaration="$6"
variant="$7"
tree="$swiftxx_tree"
if [ "$backend" = "swift" ]; then tree="$swift_tree"; fi
found=""
while IFS="$(printf '\\t')" read -r row_backend row_role row_kind row_name file; do
    if [ "$row_backend" != "$backend" ] || [ "$row_role" != "source" ] || [ "$row_name" != "$declaration" ]; then
        continue
    fi
    case "$variant:$file" in
        public:*+Private.swift) continue ;;
        private:*+Private.swift) ;;
        private:*.swift) continue ;;
        public:*.swift|source:*.cpp) ;;
        *) continue ;;
    esac
    if [ -n "$found" ]; then
        echo "More than one $backend $variant source found for $declaration" >&2
        exit 1
    fi
    found="$file"
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
                progress_message = "Selecting %s %s source for %s" % (backend, variant, name),
            )
            bucket.append(output)

    return [
        DefaultInfo(files = depset(public_swift + private_swift + swiftxx_sources)),
        OutputGroupInfo(
            swift_public = depset(public_swift),
            swift_private = depset(private_swift),
            swiftxx_sources = depset(swiftxx_sources),
        ),
    ]

_typed_swift_outputs = rule(
    implementation = _typed_swift_outputs_impl,
    attrs = {
        "djinni": attr.label(mandatory = True, providers = [DjinniInfo]),
        "owned_manifest": attr.label(allow_single_file = True, mandatory = True),
        "swift_tree": attr.label(allow_single_file = True, mandatory = True),
        "swiftxx_tree": attr.label(allow_single_file = True, mandatory = True),
    },
)

def _owned_swiftxx_headers_impl(ctx):
    output = ctx.actions.declare_directory(ctx.label.name + ".headers")
    ctx.actions.run_shell(
        inputs = [ctx.file.swiftxx_tree, ctx.file.owned_manifest],
        outputs = [output],
        arguments = [ctx.file.swiftxx_tree.path, ctx.file.owned_manifest.path, output.path],
        command = """set -euo pipefail
tree="$1"
manifest="$2"
output="$3"
mkdir -p "$output"
while IFS="$(printf '\\t')" read -r row_backend row_role row_kind row_name file; do
    if [ "$row_backend" = "swiftxx" ] && [ "$row_role" = "header" ]; then
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

_owned_swiftxx_headers = rule(
    implementation = _owned_swiftxx_headers_impl,
    attrs = {
        "owned_manifest": attr.label(allow_single_file = True, mandatory = True),
        "swiftxx_tree": attr.label(allow_single_file = True, mandatory = True),
    },
)

def swift_djinni_library(
        name,
        deps,
        module_name,
        srcs = [],
        bridge_srcs = [],
        swift_deps = [],
        bridge_deps = [],
        cc_deps = [],
        swift_copts = None,
        cc_copts = None,
        bridge_module_name = None,
        visibility = None,
        **kwargs):
    """Generates and compiles Djinni Swift and Swift/C++ bridge libraries.

    The `djinni_library` dependency must provide declaration type lists so the
    generated Swift sources can retain their per-file access scopes.
    `module_name` must match the Swift module configured for the IDL. `deps`
    contains exactly one `djinni_library` target.
    """
    if len(deps) != 1:
        fail("swift_djinni_library expects exactly one djinni_library dep")
    if not module_name:
        fail("swift_djinni_library requires module_name")
    if "swift_module" in kwargs or "swift_private_in_same_module" in kwargs:
        fail("Set Swift module options through module_name; Swift private sources use a separate module")

    codegen_name = name + "__djinni_swift_codegen"
    codegen_kwargs = dict(kwargs)
    common = {}
    for attr_name in _COMMON_ATTRS:
        if attr_name in codegen_kwargs:
            common[attr_name] = codegen_kwargs.pop(attr_name)

    native_visibility = visibility or ["//visibility:private"]
    djinni_codegen(
        name = codegen_name,
        djinni = deps[0],
        tree_languages = ["swift", "swiftxx"],
        swift_module = module_name,
        visibility = ["//visibility:private"],
        **dict(codegen_kwargs, **common)
    )

    swift_tree_name = codegen_name + "__swift_tree"
    swiftxx_tree_name = codegen_name + "__swiftxx_tree"
    owned_manifest_name = codegen_name + "__owned_manifest"
    native.filegroup(
        name = swift_tree_name,
        srcs = [":" + codegen_name],
        output_group = "swift_tree",
        visibility = ["//visibility:private"],
        **common
    )
    native.filegroup(
        name = swiftxx_tree_name,
        srcs = [":" + codegen_name],
        output_group = "swiftxx_tree",
        visibility = ["//visibility:private"],
        **common
    )
    native.filegroup(
        name = owned_manifest_name,
        srcs = [":" + codegen_name],
        output_group = "outputs_by_declaration",
        visibility = ["//visibility:private"],
        **common
    )

    typed_outputs_name = name + "__typed_outputs"
    _typed_swift_outputs(
        name = typed_outputs_name,
        djinni = deps[0],
        owned_manifest = ":" + owned_manifest_name,
        swift_tree = ":" + swift_tree_name,
        swiftxx_tree = ":" + swiftxx_tree_name,
        **common
    )
    public_swift_name = name + "__public_swift_sources"
    private_swift_name = name + "__private_swift_sources"
    swiftxx_sources_name = name + "__swiftxx_sources"
    native.filegroup(
        name = public_swift_name,
        srcs = [":" + typed_outputs_name],
        output_group = "swift_public",
        visibility = ["//visibility:private"],
        **common
    )
    native.filegroup(
        name = private_swift_name,
        srcs = [":" + typed_outputs_name],
        output_group = "swift_private",
        visibility = ["//visibility:private"],
        **common
    )
    native.filegroup(
        name = swiftxx_sources_name,
        srcs = [":" + typed_outputs_name],
        output_group = "swiftxx_sources",
        visibility = ["//visibility:private"],
        **common
    )
    swiftxx_headers_name = name + "__swiftxx_headers"
    _owned_swiftxx_headers(
        name = swiftxx_headers_name,
        owned_manifest = ":" + owned_manifest_name,
        swiftxx_tree = ":" + swiftxx_tree_name,
        **common
    )
    public_swift_srcs = [":" + public_swift_name]
    private_swift_srcs = [":" + private_swift_name]
    generated_swiftxx_srcs = [":" + swiftxx_sources_name]
    generated_swiftxx_hdrs = [":" + swiftxx_headers_name]
    swiftxx_directory = swiftxx_headers_name + ".headers"

    swiftxx_hint_name = name + "__swiftxx_hint"
    swiftxx_core_name = name + "__swiftxx_core"
    swiftxx_name = name + "__swiftxx"
    swiftxx_interop_module_name = module_name + "Cxx"
    bridge_module_name = bridge_module_name or module_name + "Bridge"
    if swift_copts == None:
        swift_copts = ["-cxx-interoperability-mode=default", "-Xcc", "-std=c++20"]
    if cc_copts == None:
        cc_copts = ["-std=c++20"]

    swift_library(
        name = name + "__swift",
        srcs = public_swift_srcs + srcs,
        copts = swift_copts,
        module_name = module_name,
        deps = [_DJINNI_SWIFT_SUPPORT] + swift_deps,
        visibility = ["//visibility:private"],
        **common
    )

    swift_interop_hint(
        name = swiftxx_hint_name,
        module_name = swiftxx_interop_module_name,
    )

    cc_library(
        name = swiftxx_core_name,
        srcs = generated_swiftxx_srcs,
        hdrs = generated_swiftxx_hdrs,
        copts = cc_copts,
        includes = [swiftxx_directory] if swiftxx_directory and swiftxx_directory != "." else [],
        deps = cc_deps + [_DJINNI_SWIFTXX_SUPPORT_CORE],
        visibility = ["//visibility:private"],
        **common
    )

    cc_library(
        name = swiftxx_name,
        hdrs = generated_swiftxx_hdrs,
        aspect_hints = [":" + swiftxx_hint_name],
        deps = [":" + swiftxx_core_name, _DJINNI_SWIFTXX_SUPPORT],
        visibility = ["//visibility:private"],
        **common
    )

    swift_library(
        name = name,
        srcs = private_swift_srcs + bridge_srcs,
        copts = swift_copts,
        module_name = bridge_module_name,
        deps = [":" + name + "__swift", ":" + swiftxx_name, _DJINNI_SWIFT_SUPPORT, _DJINNI_SWIFTXX_SUPPORT] + bridge_deps,
        visibility = native_visibility,
        **common
    )
