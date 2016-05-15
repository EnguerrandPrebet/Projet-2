#!/bin/bash
folder=$1
shift
echo "plop"
echo $*
echo "plop"
for FILE in $folder/*
do
	echo $((time ./../../bin/resol $FILE $*) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g")
done
