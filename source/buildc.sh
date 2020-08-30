#!/bin/bash
type=${1:-debug}
clean=${2:-0}
all=${3:-1}
export CXX=clang++
if [ $all -eq 1 ]; then
	../../Framework/cmake/buildc.sh ../../Framework/source $type $clean || exit 1;
	../../Framework/cmake/buildc.sh ../../Ssl/source $type $clean || exit 1;
	../../Framework/cmake/buildc.sh ../../Google/source $type $clean || exit 1;
fi
if [ ! -d .obj ];	then mkdir .obj; clean=1; fi;
cd .obj;
if [ ! -d $type ]; then mkdir $type; clean=1; fi;
cd $type;
if [ ! -f CMakeCache.txt ]; then clean=1; fi;

if [ $clean -eq 1 ]; then
	rm -f CMakeCache.txt;
	cmake -DCMAKE_BUILD_TYPE=$type  ../.. > /dev/null;
	make clean;
fi
make -j7
cd - > /dev/null
exit $?
