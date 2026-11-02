genrule(
    name = "stub",
    outs = ["stub.sh"],
    cmd = """
printf '#!/usr/bin/env sh\\n' > $@
printf '@ 2>/dev/null # 2>nul & echo off & goto BOF\\n' >> $@
printf ':\\n' >> $@
printf 'exec java -jar $${JAVA_OPTS:-} "$$0" "$$@"\n' >> $@
printf 'exit\\n' >> $@
printf '\\n' >> $@
printf ':BOF\\r\\n' >> $@
printf '@echo off\\r\\n' >> $@
printf 'java -jar %%JAVA_OPTS%% "%%~dpnx0" %%*\\r\\n' >> $@
printf 'exit /B %%errorlevel%%\\r\\n' >> $@
""",
)

genrule(
    name = "generator",
    srcs = [
        ":stub",
        "//src:djinni_deploy.jar",
    ],
    outs = ["djinni"],
    cmd = "cat $(location :stub) $(location //src:djinni_deploy.jar) > $@ && chmod +x $@",
    executable = True,
)
