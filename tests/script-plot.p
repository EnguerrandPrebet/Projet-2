reset

set term pdfcairo
set output "stats.pdf"

set title "Comparaison de DPLL avec rien, littéraux surveillés et les heuristiques"
set xlabel "1.cnf	2.cnf	3.cnf	4.cnf	5.cnf	6.cnf	7.cnf	20-91	50-218"
set ylabel "Temps d'execution"

set xrange [5:10]
set boxwidth 0.125
set style fill solid 0.5

plot [0:10] [-1:] "graph.dat" using 1:2 title columnheader(2) with boxes, \
     "graph.dat" using 3:4 title columnheader(3) with boxes, \
     "graph.dat" using 5:6 title columnheader(4) with boxes, \
     "graph.dat" using 7:8 title columnheader(5) with boxes, \
     "graph.dat" using 9:10 title columnheader(6) with boxes
     
pause -1 "Appuyez sur Entrée pour voir avec plus de précision sans les temps trop grands\n"
plot with boxes [0:10] [-0.5:] "graph.dat" every 2::1 using 1:2 title columnheader(2), \
     "graph.dat" every 2::1 using 1:3 title columnheader(3), \
     "graph.dat" every 2::1 using 1:4 title columnheader(4), \
     "graph.dat" every 2::1 using 1:5 title columnheader(5), \
     "graph.dat" every 2::1 using 1:6 title columnheader(6)
