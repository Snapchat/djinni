"""
Scala configuration for Djinni.

This file configures Scala toolchains for both WORKSPACE and Bzlmod modes.
"""

def djinni_scala_config():
    """
    Configure Scala toolchains for Djinni.
    
    In Bzlmod mode, this may be handled differently by rules_scala.
    This function is maintained for WORKSPACE compatibility.
    """
    
    # Note: In Bzlmod mode, Scala configuration is handled by rules_scala
    # This function is for WORKSPACE mode compatibility
    
    # Legacy WORKSPACE mode
    # Load and configure Scala
    native.bind(
        name = "io_bazel_rules_scala/dependency/scala/scala_library",
        actual = "@io_bazel_rules_scala//scala:scala_library_2_11",
    )
    
    # Scala version is configured via scala_config in rules_scala
    # This is typically done after loading the repository
    # The actual registration happens in setup_deps.bzl
