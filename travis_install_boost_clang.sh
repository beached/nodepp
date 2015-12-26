#!/bin/bash
set -e

if [ ! -d "${BOOST_ROOT}" ]; then
	# check to see if protobuf folder is empty
	BUILD_TYPE=clang
	BOOST_FILE="boost_1_60_0.tar.bz2"
	mkdir "/tmp/${BUILD_TYPE}"
	cd "/tmp/${BUILDTYPE}"
	wget -O "${BOOST_FILE}" "http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2/download"
	tar -xjf ${BOOST_FILE}

	echo "building boost";
	cd boost_1_60_0;

	./bootstrap.sh toolset=${BUILD_TYPE} --prefix="${BOOST_ROOT}"
	./b2 toolset=${BUILD_TYPE} cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --prefix="${BOOST_ROOT}" install; 
else
	echo "using cached boost folder"
fi

