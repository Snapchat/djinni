"""
Setup dependencies for Djinni - second phase initialization.

DEPRECATED: This file is maintained for WORKSPACE backwards compatibility.
In Bzlmod, dependency setup is handled automatically by MODULE.bazel.
"""

def djinni_setup_deps():
    """
    Second phase dependency setup for WORKSPACE mode.
    
    DEPRECATED: When using Bzlmod (MODULE.bazel), this function is not needed.
    Module extensions and the BCR handle dependency initialization automatically.
    """
    
    # Note: In Bzlmod mode, dependency setup is automatic
    # This function is for WORKSPACE mode compatibility
    
    # Legacy WORKSPACE mode - load and call setup functions
    # These can't be loaded at the top level since they depend on repositories
    # being loaded first in WORKSPACE
    
    # Note: In WORKSPACE, these loads happen after djinni_deps() is called
    # load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
    # load("@io_bazel_rules_scala//scala:toolchains.bzl", "scala_register_toolchains")
    # load("@io_bazel_rules_scala//scala:scala.bzl", "scala_repositories")
    # load("@rules_jvm_external//:defs.bzl", "maven_install")
    # load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
    
    # These calls would be made from WORKSPACE after loading this file
    pass

def _djinni_setup_deps_workspace_impl():
    """
    Internal function that should be called from WORKSPACE.
    Performs the actual setup that requires loaded dependencies.
    """
    native.local_repository(
        name = "maven_djinni",
        path = "maven_artifacts",
    )
