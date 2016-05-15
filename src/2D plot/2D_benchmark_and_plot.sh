#!/bin/bash
TIMEFORMAT=%R
SIZE=$1
shift
RESOL_OP=$*
N1=10
N2=300
N_INCR=10
M1=3
M2=4
M_INCR=1
NB_ESSAIS=10
rm -f stats.txt out.txt
for ((n=N1;n<=N2;n+=N_INCR)); do
	for ((m=M1;m<=M2;m+=M_INCR)); do
		./bin/generator $n $m $SIZE "./gen" $NB_ESSAIS
		sum=0
		for ((i=0;i<NB_ESSAIS;i++)); do
			filename="$i"
			filename+=".cnf"
			duration="$((time (./bin/resol $RESOL_OP $filename >> out.txt)) 2>&1)"
			calcul=$sum
			calcul+="+"
			calcul+=$duration
			sum=$(echo $calcul | bc)
			rm -f "$filename"
		done
		echo -n $sum >> stats.txt
		(printf " ") >> stats.txt
	done
	echo &>> stats.txt
done
gnuplot -persist plot.gnu
exit 0
