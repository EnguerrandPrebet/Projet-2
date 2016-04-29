CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

LEX=flex
LIBLEX=-lfl
YACC=bison

NAME=formula_input
CFILES=./$(NAME).tab.cpp ./$(NAME).cpp ./parser.cpp
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all create_folder

all : $(OFILES) ./../../obj/$(NAME).yy.o

include $(wildcard ../../dep/parser/*.d)

create_folder:
	mkdir -p ../../dep/parser/
	
$(OFILES) : ./../../obj/%.o : %.cpp ../../dep/parser/%.d
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $< $(LIBLEX)

./../../obj/$(NAME).yy.o : $(NAME).yy.c
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^
	
$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

../../dep/parser/%.d: %.cpp create_folder
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
