CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

CFILES=$(wildcard ./*.cpp)
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all create_folder

all : create_folder $(OFILES)

include $(wildcard ../../dep/DPLL/*.d)

create_folder:
	mkdir -p ../../dep/DPLL
	
$(OFILES): ./../../obj/%.o : %.cpp ../../dep/DPLL/%.d
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $<
	
../../dep/DPLL/%.d: %.cpp
	$(CXX) $(CPPFLAGS) -MM -MD -MT ../../obj/$*.o -o $@ $<
