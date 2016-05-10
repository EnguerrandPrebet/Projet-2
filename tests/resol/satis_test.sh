#!/bin/bash
folder=$1
shift
shift
for FILE in $folder/*
do
	res=$(./../../bin/resol $FILE $*  | head -1 | cut -c3-)
	if [ "$res" != "SATISFIABLE" ]
		then echo "$(basename $FILE), fail: $res"
	fi
done
