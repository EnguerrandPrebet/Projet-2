#!/bin/bash
for FILE in $1/*
do
	res=$(./../../bin/resol $FILE $2  | head -1 | cut -c3-)
	if [ "$res" != "SATISFIABLE" ]
		then echo "$(basename $FILE), fail: $res"
	fi
done
