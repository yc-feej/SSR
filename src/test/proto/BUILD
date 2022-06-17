load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "refl_test_proto",
    srcs = ["refl_test.proto"],
)

cc_proto_library(
    name = "refl_test_cc_proto",
    deps = [":refl_test_proto"],
)
