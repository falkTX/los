#!/bin/bash

if [ -d build-debug ]; then
        echo "Build dir already exists"
else
	echo "Create build dir"
	mkdir build-debug
fi
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make && echo "Build was OK, now enter the 'build-debug' dir and run 'make install' as root"
