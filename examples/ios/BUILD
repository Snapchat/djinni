load("@djinni_apple//:swift_djinni_codegen.bzl", "swift_djinni_library")
load("@djinni_rules//examples:djinni_outputs.bzl", "EXAMPLE_DJINNI_OUTS")

swift_djinni_library(
    name = "textsort-swift-bridge",
    cc_deps = ["@djinni_rules//examples:textsort-common"],
    cpp_namespace = "textsort",
    deps = ["@djinni_rules//examples:example-djinni"],
    ident_cpp_enum_type = "foo_bar",
    ident_swiftxx_class = "SwiftFooBar",
    ident_swiftxx_file = "SwiftFooBar",
    module_name = "TextSort",
    outs = EXAMPLE_DJINNI_OUTS,
    visibility = ["//visibility:public"],
)
