#!/bin/bash

test_dir_path=${PWD}"/test"
build_path=${PWD}"/cmake-build-debug/client"
server_address="127.0.0.1:8000"

/usr/bin/cmake --build "${build_path}" --target mini_fs_client -- -j 6
python3 "${test_dir_path}""/write_read_test.py" | "${build_path}""/mini_fs_client" "${server_address}"
python3 "${test_dir_path}""/check_write_read.py"
