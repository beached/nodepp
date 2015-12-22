#!/bin/bash
set -e

if [ ! -d "${BOOST_ROOT}_clang" ]; then
	# check to see if protobuf folder is empty
	if [ ! -d ~/boost.git ]; then
#		wget -O boost_1_60_0.tar.bz2 http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2/download
		tar -xjf boost_1_60_0.tar.bz2
	fi

	echo "building boost";
	mv boost_1_60_0 boost_1_60_0_clang
	cd boost_1_60_0_clang;

	echo "using gcc : : ${BOOST_CXX} ;" > ~/user-config.jam;
	./bootstrap.sh toolset=clang --prefix="${BOOST_ROOT}_clang"
	
	./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --prefix="${BOOST_ROOT}_clang" install; 
else
	echo "using cached boost folder"
fi

