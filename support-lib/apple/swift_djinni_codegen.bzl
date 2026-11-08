"""Bazel macro for compiling Djinni-generated Swift and Swift/C++ bridges."""

load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")
load("@build_bazel_rules_swift//swift:swift_interop_hint.bzl", "swift_interop_hint")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@djinni_rules//bzl:djinni_codegen.bzl", "djinni_codegen")

_DJINNI_SWIFT_SUPPORT = Label("//:djinni-support-swift")
_DJINNI_SWIFTXX_SUPPORT = Label("//:djinni-support-swiftxx")
_DJINNI_SWIFTXX_SUPPORT_CORE = Label("@djinni_rules//support-lib:djinni-support-swiftxx-core")
_COMMON_ATTRS = ["testonly", "tags", "compatible_with", "restricted_to", "target_compatible_with", "features", "deprecation"]

def _output_labels(outputs):
    return [":" + output for output in outputs]

def _output_directory(outputs, category):
    if not outputs:
        return None
    directory = outputs[0].rsplit("/", 1)[0] if "/" in outputs[0] else "."
    for output in outputs:
        output_directory = output.rsplit("/", 1)[0] if "/" in output else "."
        if output_directory != directory:
            fail("%s outputs must all be in one directory; got %s and %s" % (category, directory, output_directory))
    return directory

def swift_djinni_library(
        name,
        deps,
        outs,
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

    `outs` must explicitly declare the generated Swift, Swiftxx C++, and
    Swiftxx header files. `module_name` must match the Swift module configured
    for the IDL. `deps` contains exactly one `djinni_library` target.
    """
    if len(deps) != 1:
        fail("swift_djinni_library expects exactly one djinni_library dep")
    if not outs or not outs.get("swift_srcs"):
        fail("swift_djinni_library requires explicit swift_srcs in outs")
    if not module_name:
        fail("swift_djinni_library requires module_name")
    if "swift_module" in kwargs or "swift_private_in_same_module" in kwargs:
        fail("Set Swift module options through module_name; Swift private sources use a separate module")

    swift_srcs = outs.get("swift_srcs", [])
    swiftxx_srcs = outs.get("swiftxx_srcs", [])
    swiftxx_hdrs = outs.get("swiftxx_hdrs", [])
    swift_codegen_outs = {
        "swift_srcs": swift_srcs,
        "swiftxx_srcs": swiftxx_srcs,
        "swiftxx_hdrs": swiftxx_hdrs,
    }
    generated_swift_srcs = _output_labels(swift_srcs)
    public_swift_srcs = [source for source in generated_swift_srcs if not source.endswith("+Private.swift")]
    private_swift_srcs = [source for source in generated_swift_srcs if source.endswith("+Private.swift")]
    generated_swiftxx_srcs = _output_labels(swiftxx_srcs)
    generated_swiftxx_hdrs = _output_labels(swiftxx_hdrs)
    swiftxx_directory = _output_directory(swiftxx_srcs + swiftxx_hdrs, "swiftxx")

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
        outs = swift_codegen_outs,
        swift_module = module_name,
        visibility = ["//visibility:private"],
        **dict(codegen_kwargs, **common)
    )

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
