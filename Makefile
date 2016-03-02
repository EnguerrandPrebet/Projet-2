CXX=g++
CPPFLAGS=-Wall -O2 -std=c++11 -Wno-write-strings
TSEITIN=./src/tseitin/tseitin.cpp
RESOL=$(wildcard ./src/resol/*.cpp)
NAME=./src/parser/formula_input
PARSER=$(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp ./src/parser/parser.cpp
LEX=flex
LIBLEX=-lfl
YACC=bison

all: $(TSEITIN) $(RESOL) $(PARSER)
	$(CXX) $(CPPFLAGS) -o ./bin/resol $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^
	
$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output

mrpoper: clean
	rm -f /bin/resol
