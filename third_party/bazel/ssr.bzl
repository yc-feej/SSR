load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

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

    if "rules_proto" not in native.existing_rules():
        git_repository(
            name = "rules_proto",
            commit = "84ba6ec814eebbf5312b2cc029256097ae0042c3",
            remote = "git@code.deeproute.ai:third-party-repos/rules_proto.git",
            shallow_since = "1605857269 +0100",
        )

    if "zlib" not in native.existing_rules():
        new_git_repository(
            name = "zlib",
            build_file = "@com_google_protobuf//:third_party/zlib.BUILD",
            commit = "c303a52bf22635cdb50025bd3d224fe705c975fc",
            remote = "git@code.deeproute.ai:third-party-repos/zlib.git",
            shallow_since = "1585194278 +0800",
        )

    if "rules_python" not in native.existing_rules():
        git_repository(
            name = "rules_python",
            commit = "4c961d92a15f4a3f90faab82eecb18d91ee2ccbe",
            remote = "https://github.com/bazelbuild/rules_python.git",
        )

    if "com_google_protobuf" not in native.existing_rules():
        git_repository(
            name = "com_google_protobuf",
            # tag = "v3.13.0",
            commit = "fde7cf7358ec7cd69e8db9be4f1fa6a5c431386a",
            remote = "git@code.deeproute.ai:third-party-repos/protobuf.git",
            shallow_since = "1597443653 -0700",
        )
