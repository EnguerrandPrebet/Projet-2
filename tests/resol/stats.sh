#!/bin/bash

rm -f stats.dat
for FILE in ./Test_unitaire/*
do
	for i in "" -wl -rand -moms -dlis
	do
		(time ./../../bin/resol $FILE $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
	done
	echo "$(basename $FILE) is done"
done
echo "testing 20-91s' files"
for i in "" -wl -rand -moms -dlis
	do
		(time ./satis_test.sh ./20-91  $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
		echo "$i done"
	done
echo "testing 50-218s' files"
for i in "" -wl -rand -moms -dlis
	do
		(time ./satis_test.sh ./50-218  $i) 2>&1 | grep real | sed -e "s/real\t\([0-9]\)/\1/" -e "s/[ms]/\t/g" >> stats.dat
		echo "$i done"
	done
./main
gnuplot -p script-plot.p
