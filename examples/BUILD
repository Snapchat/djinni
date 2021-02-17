load("@build_bazel_rules_apple//apple:ios.bzl", "ios_application")
load("@build_bazel_rules_swift//swift:swift.bzl", "swift_library")

cc_library(
    name = "textsort-common",
    srcs = glob([
        "generated-src/cpp/*.cpp",
        "handwritten-src/cpp/*.cpp",
    ]),
    hdrs = glob([
        "generated-src/cpp/*.hpp",
        "handwritten-src/cpp/*.hpp",
    ]),
    includes = [
        "generated-src/cpp",
        "handwritten-src/cpp",
    ],
    deps = [
        "//support-lib:djinni-support-common",
    ],
    alwayslink = 1,
)

# ------------------------------------------------------------------

java_library(
    name = "textsort-java",
    srcs = glob(["generated-src/java/**/*.java"]),
    deps = ["@maven//:com_google_code_findbugs_jsr305",
            "//support-lib:djinni-support-java"],
)

cc_library(
    name = "textsort-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    includes = ["generated-src/jni"],
    deps = [":textsort-common",
            "//support-lib:djinni-support-android"],
    linkopts = ["-lm"],
    alwayslink = 1,
)

# ------------------------------------------------------------------

android_library(
    name = "textsort-activity",
    srcs = glob(["handwritten-src/java/com/dropbox/textsort/*.java"]),
    manifest = "android/app/src/main/AndroidManifest.xml",
    resource_files = glob(["android/app/src/main/res/**"]),
    deps = [":textsort-java",
            ":textsort-jni",],
    custom_package = "com.dropbox.textsort",
)

android_binary(
    name = "android-app",
    manifest = "android/AndroidManifest.xml",
    deps = [":textsort-activity"],
    custom_package = "com.dropbox.textsort",
)
