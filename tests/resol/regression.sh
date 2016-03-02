#!/bin/bash
for FILE in regression/*
do
	sol=$(head -1 "$FILE" | cut -c2-)
	res=$(./../../bin/resol $FILE  | head -1 | cut -c3-)
	text=$(head -2 "$FILE" | tail -1 | cut -c2-)
	if [ $res != $sol ]
		then	echo "$(basename $FILE), expected: $sol, \"$text\""
	fi
done
