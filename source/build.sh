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
	../../Linux/source/build.sh $debug $clean
	if [ $? -eq 1 ]; then
		exit 1
	fi
fi
if [ $clean -eq 1 ]; then
	make clean DEBUG=$debug
	if [ $debug -eq 1 ]; then
		ccache g++-8 -c -g -pthread -fPIC -std=c++17 -Wall -Wno-unknown-pragmas  -O0 -I.obj/debug -fsanitize=address -fno-omit-frame-pointer ./pc.h -o .obj/debug/stdafx.h.gch -I/home/duffyj/code/libraries/spdlog/include -I../../Framework/source -I/home/duffyj/code/libraries/json/include
	else
		ccache g++-8 -c -g -pthread -fPIC -std=c++17 -Wall -Wno-unknown-pragmas -DJDE_SSL_EXPORTS -march=native -DNDEBUG -O3 -I/home/duffyj/code/libraries/boostorg/boost_1_68_0 -I/home/duffyj/code/libraries/json/include -I/home/duffyj/code/libraries/spdlog/include -I../../Framework/source
	fi;
	if [ $? -eq 1 ]; then
		exit 1
	fi;
fi
make -j DEBUG=$debug
cd -
exit $?