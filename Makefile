SRCS=$(wildcard *.cpp)
BIN=$(SRCS:%.cpp=bin/%)

CPPFLAGS=-O2 -lm

all: $(BIN)

binDir:
	mkdir -p bin

bin/%: %.cpp | binDir
	g++ $(CPPFLAGS) $< -o $@
