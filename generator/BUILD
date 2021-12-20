genrule(
    name = "generator",
    srcs = ["stub.sh", "//src:djinni_deploy.jar"],
    outs = ["djinni"],
    cmd = "cat $(location stub.sh) $(location //src:djinni_deploy.jar) > $@ && chmod +x $@",
    tools = [],
)
