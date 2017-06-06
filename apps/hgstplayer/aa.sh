#!/bin/bash
if [ $# -ne 1 ]; then
	echo "error need arg"
	exit -1
elif [ "$1x" == "releasex" ]; then
	AA=release
elif [ "$1x" == "debugx" ]; then
	AA="debug"
else
	echo unknown arg
	exit -1
fi

echo "There are " $# " args"
echo $AA
