#!/bin/bash
set -e

if [ ! -d $BOOST_ROOT ]; then
	# check to see if protobuf folder is empty
	if [ ! -d ~/boost.git ]; then
		wget -O boost_1_59_0.tar.bz2 http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.bz2/download
		tar -xjf boost_1_59_0.tar.bz2
	fi

	echo "building boost";
	cd boost_1_59_0;

	echo "using gcc : : $BOOST_CXX ;" > ~/user-config.jam;
	./bootstrap.sh --prefix="$BOOST_ROT"
	
	./b2 --prefix="$BOOST_ROOT" install; 
else
	echo "using cached boost folder"
fi

