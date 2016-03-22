CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

TSEITIN=./src/tseitin/tseitin.cpp
RESOL=$(wildcard ./src/resol/*.cpp)

NAME=./src/parser/formula_input
PARSER=$(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp ./src/parser/parser.cpp
LEX=flex
LIBLEX=-lfl
YACC=bison

all: exec graph

exec: $(TSEITIN) $(RESOL) $(PARSER)
	$(CXX) $(CPPFLAGS) -O2 -o ./bin/resol $^ $(LIBLEX)

graph: ./src/stats/main.cpp
	$(CXX) $(CPPFLAGS) -o ./tests/resol/graph $^
gprof:$(TSEITIN) $(RESOL) $(PARSER)
	$(CXX) $(CPPFLAGS) -pg -o ./bin/resol $^ $(LIBLEX)
	 
debug:$(TSEITIN) $(RESOL) $(PARSER)
	$(CXX) $(CPPFLAGS) -g -D_GLIBCXX_DEBUG -o ./bin/resol $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^
	
$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

regression: debug
	cd tests/resol/; ./regression.sh

clean:
	rm -f $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output

mrpoper: clean
	rm -f /bin/resol
