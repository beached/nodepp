#!/bin/bash
set -e

# check to see if protobuf folder is empty
if [ ! -d ~/boost.git ]; then
	git clone --recursive https://github.com/boostorg/boost.git ~/boost.git;
	cd ~/boost.git
	git checkout boost-1.59.0
fi

echo "building boost";
cd ~/boost.git;

echo "using gcc : : $BOOST_CXX ;" > ~/user-config.jam;
./bootstrap.sh --prefix="$BOOST_ROOT" && ./b2 --prefix="$BOOST_ROOT" && ./b2 --prefix="$BOOST_ROOT" headers && ./b2 --prefix="$BOOST_ROOT" install; 
