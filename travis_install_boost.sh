#!/bin/bash
set -e

if [ ! -d $BOOST_ROOT ]; then
	# check to see if protobuf folder is empty
	if [ ! -d ~/boost.git ]; then
		git clone --recursive https://github.com/boostorg/boost.git ~/boost.git;
		cd ~/boost.git
		git checkout boost-1.59.0
	fi

	echo "building boost";
	cd ~/boost.git;

	echo "using gcc : : $BOOST_CXX ;" > ~/user-config.jam;
	./bootstrap.sh
	
	./b2 --prefix="$BOOST_ROOT" headers install; 
else
	echo "using cached boost folder"
fi

