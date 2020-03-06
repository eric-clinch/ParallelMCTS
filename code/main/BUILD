
cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = ["//Board:Board", "//Game:Game", "//MCTS:MCTS", "//UserPlayer:UserPlayer"],
    linkopts = ["-pthreads"],
)

cc_binary(
    name = "libboard.so",
    srcs = ["PythonInterface.cpp",],
    deps = ["//Board:Board", "//Game:Game", "//MCTS:MCTS",],
    linkopts = ["-pthreads"],
    linkshared=True
)
