load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "string_util",
    hdrs = ["string_util.h"],
)

cc_library(
    name = "reflection",
    srcs = [
        "reflection.cc",
    ],
    hdrs = [
        "descriptor_base.h",
        "reflection.h",
        "reflection_macros.h",
    ],
    deps = [
        ":string_util",
        "@boost_dynamic//:boost",
        "@glm",
    ],
)
