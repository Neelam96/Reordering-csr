# generic rules borrowed from http://nuclear.mutantstargoat.com/articles/make/

# scan for all source and dependencies
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDENCIES = $(OBJECTS:.o=.d)
VTUNE_STATIC_LIB = /opt/intel/vtune_amplifier/lib64/libittnotify.a

CC = gcc
CXX = g++
INCLUDE_DIRS = -I./src -I/opt/intel/vtune_amplifier/include
# override CXXFLAGS += -O3 -fp-model precise -g -std=c++11 $(INCLUDE_DIRS)
override CXXFLAGS += -O3  -g -std=c++11 $(INCLUDE_DIRS)
CFLAGS = $(CXXFLAGS)
LDFLAGS = -lm
EXE = pagerank

# *.o from *.c automatically recognized by automake
# only link object files required
$(EXE): $(OBJECTS) $(VTUNE_STATIC_LIB)
	$(CXX) $^ -o $@ $(LDFLAGS)

# include all dependencies in a Makefile
-include $(DEPENDENCIES)

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(DEPENDENCIES) $(OBJECTS) $(EXE)
