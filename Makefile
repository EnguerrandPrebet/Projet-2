CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings
LEX=flex
LIBLEX=-lfl
YACC=bison

EXEC=./bin/resol ./bin/graph
FOLDER= ./src/resol ./src/parser ./src/tseitin

TSEITIN=./src/tseitin/tseitin.cpp
RESOL=$(wildcard ./src/resol/*.cpp)

NAME=./src/parser/formula_input
PARSER=$(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp ./src/parser/parser.cpp

.PHONY : all $(FOLDER) gprof debug regression clean mrproper
all: $(EXEC)


$(FOLDER):
	cd $@ && make

./bin/resol: $(FOLDER)
	mkdir -p ./obj
	$(CXX) $(CPPFLAGS) -O2 -o $@ ./obj/* $(LIBLEX)

./bin/graph: ./src/stats/main.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^
	
gprof: $(FOLDER)
	$(CXX) $(CPPFLAGS) -pg -o ./bin/resol ./obj/* $(LIBLEX)
	 
debug: $(FOLDER)
	$(CXX) $(CPPFLAGS) -g -D_GLIBCXX_DEBUG -o ./bin/resol ./obj/* $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^
	
$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

regression: debug
	cd tests/resol/; ./regression.sh

clean:
	rm -f $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output ./obj/*.o

mrproper: clean
	rm -f $(EXEC)
