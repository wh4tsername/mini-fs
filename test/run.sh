#!/bin/bash

test_dir_path=${PWD}"/test"
build_path=${PWD}"/cmake-build-debug"
fs_path=${PWD}"/fs.bin"

/snap/clion/139/bin/cmake/linux/bin/cmake --build "${build_path}" --target mini_fs -- -j 6
python3 "${test_dir_path}""/write_read_test.py" | "${build_path}""/mini_fs" "${fs_path}"
python3 "${test_dir_path}""/check_write_read.py"
