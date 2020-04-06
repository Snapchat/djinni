cc_library(
    name = "jni_headers",
    hdrs = [
        "@local_jdk//:jni_header",
        "@local_jdk//:jni_md_header-darwin",
    ],
    includes = [
        "external/local_jdk/include",
        "external/local_jdk/include/darwin",
    ],
    linkstatic = 1,
    visibility = [
        "//visibility:public",
    ],
)
