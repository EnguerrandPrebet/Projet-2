CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

LEX=flex
LIBLEX=-lfl
YACC=bison

NAME=formula_input
CFILES=./$(NAME).tab.cpp ./$(NAME).cpp ./parser.cpp
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all

all : $(OFILES) ./../../obj/$(NAME).yy.o

$(OFILES) : ./../../obj/%.o : %.cpp
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $^ $(LIBLEX)

./../../obj/$(NAME).yy.o : $(NAME).yy.c
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^
	
$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

