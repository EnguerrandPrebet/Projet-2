#!/bin/bash
folder=$1
shift
for FILE in $folder/*
do
	res=$(./../../bin/resol $FILE $*  | head -1 | cut -c3-)
	if [ "$res" != "UNSATISFIABLE" ]
		then echo "$(basename $FILE), fail: $res"
	fi
done
