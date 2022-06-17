load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "reflection_test",
    srcs = ["test.cc"],
    hdrs = ["test.h"],
    deps = [
        "//src:reflection",
        "//src/test/proto:refl_test_cc_proto",
    ],
)

cc_binary(
    name = "reflection_example_main",
    srcs = ["example_main.cc"],
    deps = [
        ":reflection_test",
    ],
)
