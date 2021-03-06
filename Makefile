CXX=g++
CPPFLAGS=-pedantic -Wall -std=c++11 -Wno-write-strings
LEX=flex
LIBLEX=-lfl
YACC=bison

EXEC=./bin/resol ./bin/graph ./bin/generator
FOLDER=./src/parser ./src/tseitin ./src/DPLL ./src/interface


TSEITIN=$(wildcard ./src/tseitin/*.cpp)
INTERFACE=$(wildcard ./src/interface/*.cpp)
DPLL=$(wildcard ./src/DPLL/*.cpp)
SMT=./src/parser/smt
STD=./src/parser/std
PARSER=$(STD).tab.cpp $(STD).yy.c $(SMT).tab.cpp $(SMT).yy.c ./src/parser/formula_input.cpp ./src/parser/parser_smt.cpp ./src/parser/parser_std.cpp

.PHONY : all $(FOLDER) gprof debug regression clean mrproper

all: $(EXEC)

create_folder:
	mkdir -p ./obj
	mkdir -p ./bin
	mkdir -p ./dep

$(FOLDER): create_folder
	cd $@ && make all

./bin/resol: $(FOLDER)
	$(CXX) $(CPPFLAGS) -O2 -o $@ ./obj/* $(LIBLEX)

./bin/%: ./src/stats/%.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^

gprof:
	$(CXX) $(CPPFLAGS) -pg -o ./bin/resol $(TSEITIN) $(DPLL) $(INTERFACE) $(PARSER) $(LIBLEX)

debug:
	$(CXX) $(CPPFLAGS) -g -o ./bin/resol $(TSEITIN) $(DPLL) $(INTERFACE) $(PARSER) $(LIBLEX)

clang:
	clang++ $(CPPFLAGS) -O2 -o ./bin/resol $(TSEITIN) $(DPLL) $(INTERFACE) $(PARSER) $(LIBLEX)

$(STD).yy.c :  $(STD).l
	$(LEX) --prefix=stdd -o $@ $^ 
$(STD).tab.cpp : $(STD).ypp
	$(YACC) --name-prefix=stdd --report=all -o $@ -d $^

$(SMT).yy.c :  $(SMT).l
	$(LEX) --prefix=smt -o $@ $^ 
$(SMT).tab.cpp : $(SMT).ypp
	$(YACC) --name-prefix=smt --report=all -o $@ -d $^
	
regression: debug
	cd tests/resol/; ./regression.sh

clean:
	rm -f $(STD).yy.c $(STD).tab.cpp $(STD).tab.hpp $(STD).output
	rm -f $(SMT).yy.c $(SMT).tab.cpp $(SMT).tab.hpp $(SMT).output
	rm -f -r ./dep/ ./obj/

rebuild: mrproper all

mrproper: clean
	rm -f -r ./bin/
