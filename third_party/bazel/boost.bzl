def load_system_boost():
    if "boost_static" not in native.existing_rules():
        native.new_local_repository(
            name = "boost_static",
            build_file_content = """
cc_library(
    name = "boost",
    srcs = glob(
        ["lib/x86_64-linux-gnu/libboost*.a"],
        exclude = [
            "lib/x86_64-linux-gnu/libboost*test*.a",
            "lib/x86_64-linux-gnu/libboost_prg_exec_monitor.a",
        ],
    ),
    hdrs = glob(
        [
            "include/boost/**/*.hpp",
            "include/boost/*.hpp",
        ],
        exclude = ["include/boost/test/**"],
    ),
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
    alwayslink = 1,
    linkstatic = 1,
)
        """,
            path = "/usr",
        )

    if "boost_dynamic" not in native.existing_rules():
        native.new_local_repository(
            name = "boost_dynamic",
            build_file_content = """
cc_library(
    name = "boost",
    srcs = glob(
        [
            "lib/x86_64-linux-gnu/libboost*.so",
            "lib/x86_64-linux-gnu/libpython2.7.so.1.0",
        ],
        exclude = [
            "lib/x86_64-linux-gnu/libboost*test*.so",
            "lib/x86_64-linux-gnu/libboost_prg_exec_monitor.so",
            "lib/x86_64-linux-gnu/*python3*",
            "lib/x86_64-linux-gnu/*py3*",
        ],
    ),
    hdrs = glob(
        [
            "include/boost/**/*.hpp",
            "include/boost/*.hpp",
        ],
        exclude = ["include/boost/test/**"],
    ),
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
    alwayslink = 1,
    linkstatic = 1,
)
        """,
            path = "/usr",
        )
