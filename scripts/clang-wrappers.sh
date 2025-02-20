#!/bin/bash

REAL_CLANG="/usr/bin/clang"

function compile() {
    local SOURCE_FILE=""
    for arg in "$@"; do
        if [[ $arg == *.cpp || $arg == *.cxx ]]; then
            SOURCE_FILE=$arg
            break
        fi
    done
    if [[ -n "$SOURCE_FILE" ]]; then
        echo "CC $SOURCE_FILE"
    fi
    exec $REAL_CLANG "$@"
}

compile "$@"
