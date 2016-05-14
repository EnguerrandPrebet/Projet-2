CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

LEX=flex
LIBLEX=-lfl
YACC=bison


CFILES=./smt.tab.cpp ./std.tab.cpp ./formula_input.cpp ./parser_smt.cpp ./parser_std.cpp
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all create_folder

all : create_folder $(OFILES) ./../../obj/smt.yy.o ./../../obj/std.yy.o

include $(wildcard ../../dep/parser/*.d)

create_folder:
	mkdir -p ../../dep/parser/
	
$(OFILES) : ./../../obj/%.o : %.cpp ../../dep/parser/%.d
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $< $(LIBLEX)

./../../obj/%.yy.o : %.yy.c
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $^ $(LIBLEX)

smt.yy.c :  smt.l
	$(LEX) --prefix=smt -o $@ $^
	
smt.tab.cpp : smt.ypp
	$(YACC) --name-prefix=smt --report=all -o $@ -d $^

std.yy.c :  std.l
	$(LEX) --prefix=stdd -o $@ $^
	
std.tab.cpp : std.ypp
	$(YACC) --name-prefix=stdd --report=all -o $@ -d $^

../../dep/parser/%.d: %.cpp
	$(CXX) $(CPPFLAGS) -MM -MD -MT ../../obj/$*.o -o $@ $<
