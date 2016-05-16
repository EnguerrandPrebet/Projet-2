#!/bin/bash
folder=$1
shift
if [ "${folder: -1}" == "u" ]; then
	expected="UNSATISFIABLE"
else
	expected="SATISFIABLE"
fi
for FILE in $folder/*
do
	res=$(../bin/resol $FILE $*  | head -1 | cut -c3-)
	if [ "$res" != $expected ]
		then echo "$(basename $FILE), fail: $res"
	fi
done
