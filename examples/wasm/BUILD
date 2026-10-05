load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")

wasm_cc_binary(
    name = "wasm",
    cc_target = "@djinni_rules//examples:textsort-wasm",
)

wasm_cc_binary(
    name = "wasm-generated",
    cc_target = "@djinni_rules//examples:textsort-wasm-generated",
)

wasm_cc_binary(
    name = "perftest-wasm",
    cc_target = "@djinni_rules//perftest:perftest-wasm",
)

wasm_cc_binary(
    name = "perftest-wasm-generated",
    cc_target = "@djinni_rules//perftest:perftest-wasm-generated",
)

filegroup(
    name = "codegen-consumer-verification",
    srcs = [
        ":perftest-wasm-generated",
        ":wasm-generated",
    ],
)

sh_binary(
    name = "server",
    srcs = ["server.sh"],
    args = [
        "examples",
        "demo",
        "$(rootpaths @djinni_rules//examples:web-sources)",
        "$(rootpaths :wasm)",
    ],
    data = [
        ":wasm",
        "@djinni_rules//examples:web-sources",
    ],
)

sh_binary(
    name = "perftest-server",
    srcs = ["server.sh"],
    args = [
        "perftest",
        "perftest",
        "$(rootpaths @djinni_rules//perftest:web-sources)",
        "$(rootpaths @djinni_rules//support-lib:ts-sources)",
        "$(rootpaths :perftest-wasm)",
    ],
    data = [
        ":perftest-wasm",
        "@djinni_rules//perftest:web-sources",
        "@djinni_rules//support-lib:ts-sources",
    ],
)
