CXX=g++
CPPFLAGS=-pedantic -Wall -std=c++11 -Wno-write-strings
LEX=flex
LIBLEX=-lfl
YACC=bison

EXEC=./bin/resol ./bin/graph ./bin/generator
FOLDER=./src/parser ./src/tseitin ./src/resol

TSEITIN=./src/tseitin/tseitin.cpp
RESOL=$(wildcard ./src/resol/*.cpp)

NAME=./src/parser/formula_input
PARSER=$(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp ./src/parser/parser.cpp

.PHONY : all $(FOLDER) gprof debug regression clean mrproper
all: $(EXEC)


$(FOLDER):
	mkdir -p ./obj
	cd $@ && make

./bin/resol: $(FOLDER)

	$(CXX) $(CPPFLAGS) -O2 -o $@ ./obj/* $(LIBLEX)

./bin/%: ./src/stats/%.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^

gprof:
	$(CXX) $(CPPFLAGS) -pg -o ./bin/resol $(TSEITIN) $(RESOL) $(PARSER) $(LIBLEX)

debug:
	$(CXX) $(CPPFLAGS) -g -o ./bin/resol $(TSEITIN) $(RESOL) $(PARSER) $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

regression: debug
	cd tests/resol/; ./regression.sh

clean:
	rm -f $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output ./obj/*.o

rebuild: mrproper all

mrproper: clean
	rm -f $(EXEC)
