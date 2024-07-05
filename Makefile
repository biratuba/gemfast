SRCS=$(wildcard *.cpp)
BIN=$(SRCS:%.cpp=bin/%)

all: $(BIN)

binDir:
	mkdir -p bin

bin/%: %.cpp | binDir
	g++ -O2 $< -o $@
