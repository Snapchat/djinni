load("@io_bazel_rules_kotlin//kotlin:android.bzl", "kt_android_library")
load("@rules_android//rules:rules.bzl", "android_binary", "android_library")

android_library(
    name = "textsort-activity",
    srcs = ["@djinni_rules//examples:activity-sources"],
    custom_package = "com.dropbox.textsort",
    manifest = "app/src/main/AndroidManifest.xml",
    resource_files = glob(
        ["app/src/main/res/**"],
        exclude = ["app/src/main/res/.DS_Store"],
    ),
    deps = ["@djinni_rules//examples:textsort-java"],
)

android_binary(
    name = "android-app",
    custom_package = "com.dropbox.textsort",
    manifest = "AndroidManifest.xml",
    deps = [
        ":textsort-activity",
        "@djinni_rules//examples:textsort-jni",
    ],
)

kt_android_library(
    name = "benchmark-activity",
    srcs = ["@djinni_rules//perftest:activity-sources"],
    custom_package = "com.snapchat.djinni.benchmark",
    manifest = "@djinni_rules//perftest:android/app/src/main/AndroidManifest.xml",
    resource_files = ["@djinni_rules//perftest:android-resources"],
    deps = ["@djinni_rules//perftest:benchmark-java"],
)

android_binary(
    name = "perftest",
    custom_package = "com.snapchat.djinni.benchmark",
    manifest = "@djinni_rules//perftest:android/AndroidManifest.xml",
    deps = [
        ":benchmark-activity",
        "@djinni_rules//perftest:benchmark-jni",
    ],
)
