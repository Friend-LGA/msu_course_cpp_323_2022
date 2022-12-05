TARGET = run
BIN = ./bin
FLAGS=-std=c++17 -Wall -Werror

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(TARGET) $(BIN)/*.o

main.o: main.cpp
	clang++ $(FLAGS) -c -o $(BIN)/main.o main.cpp

$(TARGET): main.o
	clang++ $(FLAGS) -o run $(BIN)/main.o
