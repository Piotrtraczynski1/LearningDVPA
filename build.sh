#!/bin/bash

set -euo pipefail

BUILD_DIR="build"
EXTRA_FLAGS="-DENABLE_HARD_DIAGNOSTICS=OFF"

if [[ "${1-}" == "-d" ]]; then
  EXTRA_FLAGS="-DENABLE_HARD_DIAGNOSTICS=ON"
  BUILD_DIR="build-debug"
  shift
fi

build_code() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DBUILD_TESTS=OFF $EXTRA_FLAGS ..
    cmake --build .
    cd ..
}

build_and_run_tests() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DBUILD_TESTS=ON $EXTRA_FLAGS ..
    cmake --build .
    ctest --output-on-failure
    cd ..
}

build_and_run_single_test() {
    if [ -z "${1-}" ]; then
        echo "Usage: $0 [-d] test -t <TEST_NAME>"
        exit 1
    fi

    TEST_NAME="$1"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DBUILD_TESTS=ON $EXTRA_FLAGS ..
    cmake --build . --target "$TEST_NAME"

    TEST_EXEC=$(find . -type f -name "$TEST_NAME" -perm -111 | head -n1)

    if [ -z "$TEST_EXEC" ]; then
        echo "Unknown test name: $TEST_NAME"
        exit 1
    fi

    "$TEST_EXEC"
    cd ..
}

main() {
    mkdir -p "$BUILD_DIR"

    if [[ "${1-}" == "test" ]]; then
        if [[ "${2-}" == "-t" ]]; then
            build_and_run_single_test "${3-}"
        else
            build_and_run_tests
        fi
    else
        build_code
    fi
}

main "$@"