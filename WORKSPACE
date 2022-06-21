# Workspace name definition
workspace(name = "ssr")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# bazel rules
git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "1.1.1",
)

load("@bazel_skylib//lib:versions.bzl", "versions")

versions.check(minimum_bazel_version = "4.2.1")

load("@ssr//third_party/bazel:boost.bzl", "load_system_boost")
load("@ssr//third_party/bazel:ssr.bzl", "ssr_deps")

load_system_boost()

ssr_deps()
