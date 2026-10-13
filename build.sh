#!/bin/bash

set -euo pipefail

BUILD_DIR="build"
EXTRA_FLAGS="-DENABLE_HARD_DIAGNOSTICS=OFF"
EQUIVALENCE_QUERY_FLAG=""

while [[ "${1-}" == "-d" || "${1-}" == "-randomEQ" || "${1-}" == "-mixedEQ" ]]; do
    case "$1" in
    -d)
        EXTRA_FLAGS="-DENABLE_HARD_DIAGNOSTICS=ON"
        BUILD_DIR="build-debug${BUILD_DIR#build}"
        ;;
    -randomEQ)
        if [[ -n "$EQUIVALENCE_QUERY_FLAG" ]]; then
            echo "Only one of -randomEQ and -mixedEQ may be used."
            exit 1
        fi
        EQUIVALENCE_QUERY_FLAG="-DRANDOM_EQUIVALENCE_QUERY=ON"
        BUILD_DIR="${BUILD_DIR}-random"
        ;;
    -mixedEQ)
        if [[ -n "$EQUIVALENCE_QUERY_FLAG" ]]; then
            echo "Only one of -randomEQ and -mixedEQ may be used."
            exit 1
        fi
        EQUIVALENCE_QUERY_FLAG="-DMIXED_EQUIVALENCE_QUERY=ON"
        BUILD_DIR="${BUILD_DIR}-mixed"
        ;;
    esac
    shift
done

build_code() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF $EXTRA_FLAGS $EQUIVALENCE_QUERY_FLAG ..
    cmake --build .
    cd ..
}

build_and_run_tests() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON $EXTRA_FLAGS $EQUIVALENCE_QUERY_FLAG ..
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
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON $EXTRA_FLAGS $EQUIVALENCE_QUERY_FLAG ..
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
