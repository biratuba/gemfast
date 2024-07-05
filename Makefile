SRCS=$(wildcard *.cpp)
BIN=$(SRCS:.cpp=)

all: $(BIN)

binDir:
	mkdir -p bin

%: %.cpp | binDir
	g++ -O2 $< -o bin/$@
