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
    deps = [
        "//support-lib:djinni-support-java",
        "@maven_djinni//:com_google_code_findbugs_jsr305",
    ],
)

cc_library(
    name = "textsort-jni",
    srcs = glob(["generated-src/jni/*.cpp"]),
    hdrs = glob(["generated-src/jni/*.hpp"]),
    includes = ["generated-src/jni"],
    linkopts = ["-lm", "-ldl"],
    deps = [
        ":textsort-common",
        "//support-lib:djinni-support-android",
    ],
    alwayslink = 1,
)

# ------------------------------------------------------------------

android_library(
    name = "textsort-activity",
    srcs = glob(["handwritten-src/java/com/dropbox/textsort/*.java"]),
    custom_package = "com.dropbox.textsort",
    manifest = "android/app/src/main/AndroidManifest.xml",
    resource_files = glob(["android/app/src/main/res/**"]),
    deps = [":textsort-java",],
)

android_binary(
    name = "android-app",
    custom_package = "com.dropbox.textsort",
    manifest = "android/AndroidManifest.xml",
    deps = [":textsort-activity", ":textsort-jni"],
)
