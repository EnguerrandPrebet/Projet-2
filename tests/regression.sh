#!/bin/bash
shift
options1=("" "-cl")
options2=("" "-wl")
options3=("" "-rand" "-moms" "-dlis")
for option1 in "${options1[@]}"
do
	for option2 in "${options2[@]}"
	do
		for option3 in "${options3[@]}"
		do
			for FILE in regression/*
			do
				if [ "${FILE: -1}" == "r" ]
				then
					sol="SATISFIABLE"
					if [ "$(basename $FILE)" == "smt.for" ]
					then
						res=$(../bin/resol -smte $options1 $option2 $option3 $FILE $*  | head -1 | cut -c3-)
					else
						res=$(../bin/resol -t $options1 $option2 $option3 $FILE $*  | head -1 | cut -c3-)				
					fi
				else
					sol=$(head -1 "$FILE" | cut -c2-)
					res=$(../bin/resol $options1 $option2 $option3 $FILE $*  | head -1 | cut -c3-)
				fi
				
				text=$(head -2 "$FILE" | tail -1 | cut -c2-)
				if [ "$res" != "$sol" ]
				then	echo "[$option1 $option2 $option3] $(basename $FILE), expected: $sol, \"$text\""				
				fi
			done
		done
	done
done

read -p "Press [Enter] key to start more regressions (may take a while)..."
echo""
./more_regression.sh | more

