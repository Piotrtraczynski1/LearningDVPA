#!/bin/bash

BUILD_DIR="build"

build_code() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -DBUILD_TESTS=OFF ..
    cmake --build .
    cd ..
}

build_and_run_tests() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -DBUILD_TESTS=ON ..
    cmake --build .
    ctest
    cd ..
}

build_and_run_single_test() {
    if [ -z "$1" ]; then
        exit 1
    fi

    TEST_NAME="$1"
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -DBUILD_TESTS=ON ..
    cmake --build . --target "$TEST_NAME"

    TEST_EXEC=$(find . -type f -name "$TEST_NAME" -executable)

    if [ -z "$TEST_EXEC" ]; then
        echo "Nie znaleziono pliku wykonywalnego testu: $TEST_NAME"
        exit 1
    fi

    ./$TEST_EXEC
    cd ..
}

main() {
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir $BUILD_DIR
    fi

    if [ "$1" == "test" ]; then
        if [ "$2" == "-t" ]; then
            build_and_run_single_test "$3"
        else
            build_and_run_tests
        fi
    else
        build_code
    fi
}

main "$@"
