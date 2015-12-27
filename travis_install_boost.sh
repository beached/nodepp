#!/bin/bash
set -e
BOOST_VERSION="1_60_0"

# PREREQS:  BOOST_ROOT, CC, CXXFLAGS, LINKFLAGS

echo "Boost Root is set to: '${BOOST_ROOT}'"
if [ ! -d "${BOOST_ROOT}" ]; then
	BUILD_TYPE=${CC}
	BOOST_FILE="boost_${BOOST_VERSION}.tar.bz2"
	mkdir "/tmp/${BUILD_TYPE}"
	cd "/tmp/${BUILDTYPE}"
	wget -O "${BOOST_FILE}" "http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_${BOOST_VERSION}.tar.bz2/download"
	tar -xjf ${BOOST_FILE}

	echo "building boost";
	cd "boost_${BOOST_VERSION}";
	echo "Building boost in '`pwd`'"
	./bootstrap.sh toolset=${CC} --prefix="${BOOST_ROOT}"
	./b2 toolset=${CC} cxxflags="${CXXFLAGS}" linkflags="${LINKFLAGS}" --prefix="${BOOST_ROOT}" install;
else
	echo "using cached boost folder"
fi

