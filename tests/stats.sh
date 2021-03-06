#!/bin/bash

rm -f stats.dat
for FILE in ./benchmarks/*
do
	for i in "" -wl -rand -moms -dlis -cl -vsids
	do
		(time ./../bin/resol $FILE $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
	done
	echo "$(basename $FILE) is done"
done
echo "testing 20-91s' files"
for i in "" -wl -rand -moms -dlis -cl -vsids
	do
		(time ./sat_folder.sh ./more_regression/20-91  $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
		echo "$i done"
	done
echo "testing 50-218s' files"
for i in "" -wl -rand -moms -dlis -cl -vsids
	do
		(time ./sat_folder.sh ./more_regression/50-218  $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
		echo "$i done"
	done
./../bin/graph
gnuplot -p script-plot.p
