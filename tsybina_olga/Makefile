TARGET = run
BIN = ./bin
FLAGS=-std=c++17 -Wall -Werror

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(TARGET) $(BIN)/*.o

graph_generation.o: graph_generation.cpp
	clang++ $(FLAGS) -c -o $(BIN)/graph_generation.o graph_generation.cpp

$(TARGET): graph_generation.o
	clang++ $(FLAGS) -o run $(BIN)/graph_generation.o
