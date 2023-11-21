## Building the Android example app

First make sure that the ANDROID_HOME and ANDROID_NDK_HOME environment variables are set and pointing to working installations of the Android SDK and NDK, respectively.

Build with bazel: `bazel build //examples:android-app`.

Build and deploy to device: `bazel mobile-install //examples:android-app`.

## Building the iOS example app

Open the project in Xcode(tested with 12.3), then press `Cmd-B` to build.

Press the ▶ toolbar button to run it in emulator.
