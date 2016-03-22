reset

set title "Comparaison de DPLL avec rien, littéraux surveillés et les heuristiques"
set xlabel "Tests"
set ylabel "Temps dexecution"

set style data linespoints

set pointsize 2

plot [0:10] [-1:]"graph.dat" using 1:2 title columnheader(2), \
     "graph.dat" using 1:3 title columnheader(3), \
     "graph.dat" using 1:4 title columnheader(4), \
     "graph.dat" using 1:5 title columnheader(5), \
     "graph.dat" using 1:6 title columnheader(6)
