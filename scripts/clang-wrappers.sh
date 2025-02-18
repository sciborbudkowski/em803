#!/bin/bash

REAL_CLANG="/usr/bin/clang"

function compile {
	SOURCE_FILE=""
	for arg in "$@"; do
		if [[ $arg == *.cpp || $arg == *.cxx ]]; then
			SOURCE_FILE=$arg
			break
		fi
	done

	if [[ ! -z "$SOURCE_FILE" ]]; then
		echo "CC $source_file"
	fi

	$REAL_CLANG "$@"
}

compile "$@"
