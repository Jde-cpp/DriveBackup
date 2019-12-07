#!/bin/bash
debug=${1:-1}
all=${2:-1}
clean=${3:-0}
echo "Build Drive.Backup"
cd "${0%/*}"
if [ $all -eq 1 ]; then
	../../Framework/source/build.sh $debug $clean
	if [ $? -eq 1 ]; then
		exit 1
	fi
	../../Ssl/source/build.sh $debug $clean
	if [ $? -eq 1 ]; then
		exit 1
	fi
	../../Google/source/build.sh $debug $clean
	if [ $? -eq 1 ]; then
		exit 1
	fi
fi
if [ $clean -eq 1 ]; then
	make clean DEBUG=$debug
	if [ $debug -eq 1 ]; then
		ccache g++-8 -c -g -pthread -fPIC -std=c++17 -Wall -Wno-unknown-pragmas  -O0 -fsanitize=address -fno-omit-frame-pointer ./pc.h -o .obj/debug2/stdafx.h.gch -I$BOOST_ROOT -I/home/duffyj/code/libraries/spdlog/include -I../../Framework/source -I/home/duffyj/code/libraries/json/include
	else
		ccache g++-8 -c -g -pthread -fPIC -std=c++17 -Wall -Wno-unknown-pragmas -DNDEBUG -O3 ./pc.h -o .obj/release2/stdafx.h.gch -I$BOOST_ROOT -I/home/duffyj/code/libraries/spdlog/include -I../../Framework/source -I/home/duffyj/code/libraries/json/include
	fi;
	if [ $? -eq 1 ]; then
		exit 1
	fi;
fi
make -j DEBUG=$debug
cd -
exit $?