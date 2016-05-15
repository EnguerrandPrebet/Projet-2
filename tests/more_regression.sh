#!/bin/bash
shift
for folder in more_regression/*
do
	if [ "${folder: -1}" == "u" ]; then
		expected="UNSATISFIABLE"
	else
		expected="SATISFIABLE"
	fi
	
	echo "TESTING : $folder"
	for file in "$folder"/*
	do
		res=$(./../../bin/resol $file $*  | head -1 | cut -c3-)
		if [ "$res" != "$expected" ]
			then echo "$file, expected: $expected"
		fi
	done
done
exit 0
