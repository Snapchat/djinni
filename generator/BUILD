genrule(
    name = "generator",
    srcs = ["stub.sh", "//src:djinni.jar"],
    outs = ["djinni"],
    cmd = "cat $(location stub.sh) $(location //src:djinni.jar) > $@ && chmod +x $@",
    tools = [],
)
