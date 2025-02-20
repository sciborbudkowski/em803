#!/bin/bash

set -e

REAL_CXX="/usr/bin/g++"

function compile() {
    local SOURCE_FILE=""
    for arg in "$@"; do
        if [[ $arg == *.cpp || $arg == *.cxx ]]; then
            SOURCE_FILE=$arg
            break
        fi
    done
    if [[ -n "$SOURCE_FILE" ]]; then
        echo "CXX $SOURCE_FILE"
    fi
    exec $REAL_CXX "$@"
}

compile "$@"
