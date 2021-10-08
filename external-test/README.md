External Test
-------------
This folder provides a standalone workspace to mimic consuming Djinni from an external repository via Bazel.
For now, this simply tests that we can build and run the compiler. Eventually a more complete setup will be
exercised here. To run, navigate to this folder and then use:
`bazel run @djinni//src:djinni -- <command line flags>`
