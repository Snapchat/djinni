load("@djinni_apple//:swift_djinni_codegen.bzl", "swift_djinni_library")

swift_djinni_library(
    name = "textsort-swift-bridge",
    cc_deps = ["@djinni_rules//examples:textsort-common"],
    cpp_namespace = "textsort",
    deps = ["@djinni_rules//examples:example-djinni"],
    ident_cpp_enum_type = "foo_bar",
    ident_swiftxx_class = "SwiftFooBar",
    ident_swiftxx_file = "SwiftFooBar",
    module_name = "TextSort",
    visibility = ["//visibility:public"],
)
