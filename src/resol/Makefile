CXX=g++
CPPFLAGS=-Wall -std=c++11 -Wno-write-strings

CFILES=$(wildcard ./*.cpp)
OFILES=$(subst ./,./../../obj/,$(CFILES:.cpp=.o))

.PHONY: all create_folder

all : $(OFILES)

include $(wildcard ../../dep/resol/*.d)

create_folder:
	mkdir -p ../../dep/resol
	
$(OFILES): ./../../obj/%.o : %.cpp ../../dep/resol/%.d
	$(CXX) -O2 $(CPPFLAGS) -c -o $@ $<
	
../../dep/resol/%.d: %.cpp create_folder
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

