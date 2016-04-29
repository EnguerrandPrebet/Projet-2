CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

CFILES=$(wildcard ./*.cpp)
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all create_folder

all : $(OFILES)

create_folder:
	mkdir -p ../../dep/tseitin/
	
include $(wildcard ../../dep/tseitin/*.d)

$(OFILES): ./../../obj/%.o : %.cpp ../../dep/tseitin/%.d
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $<
	
../../dep/tseitin/%.d: %.cpp create_folder
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
