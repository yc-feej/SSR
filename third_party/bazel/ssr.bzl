load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

def ssr_deps():
    if "glm" not in native.existing_rules():
        new_git_repository(
            name = "glm",
            commit = "6ad79aae3eb5bf809c30bf1168171e9e55857e45",
            remote = "https://github.com/g-truc/glm.git",
            build_file = "//third_party:glm_ext.BUILD",
        )

    if "nlohmann" not in native.existing_rules():
        new_git_repository(
            name = "nlohmann",
            commit = "41226d0a03ef3e1f832af4c5cdda1e354fa4fce0",
            remote = "https://github.com/nlohmann/json.git",
            build_file = "//third_party:nlohmann.BUILD",
        )
