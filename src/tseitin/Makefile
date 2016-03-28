CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

CFILES=$(wildcard ./*.cpp)
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all

all : $(OFILES)

$(OFILES): ./../../obj/%.o : %.cpp
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $^
