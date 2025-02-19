#!/bin/bash

REAL_CXX="/usr/bin/g++"

function compile {
	SOURCE_FILE=""
	for arg in "$@"; do
		if [[ $arg == *.cpp || $arg == *.cxx ]]; then
			SOURCE_FILE=$arg
			break
		fi
	done

	if [[ ! -z "$SOURCE_FILE" ]]; then
		echo "CXX $source_file"
	fi

	$REAL_CXX "$@"
}

compile "$@"
