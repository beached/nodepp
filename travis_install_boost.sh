#!/bin/bash
set -e

# check to see if protobuf folder is empty
if [ ! -d "$BOOST_ROOT" ]; then
	echo "building boost"
	alias gcc='${CC}';
	alias g++='${CXX}';
	wget -O boost_1_59_0.tar.bz2 http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.bz2/download;
	tar -xjf boost_1_59_0.tar.bz2;
	cd boost_1_59_0 && ./bootstrap.sh --prefix="$BOOST_ROOT" && ./b2 -d0 -q install; 
else
	echo 'Using cached boost directory.';
fi