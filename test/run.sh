#!/bin/bash

test_dir_path="/home/wasd/github/mini-fs/test"
build_path="/home/wasd/github/mini-fs/cmake-build-debug"
fs_path="/home/wasd/github/mini-fs/fs.bin"

/snap/clion/139/bin/cmake/linux/bin/cmake --build ${build_path} --target mini_fs -- -j 6
python3 ${test_dir_path}"/test.py" | ${build_path}"/mini_fs" ${fs_path}
python3 ${test_dir_path}"/check.py"
