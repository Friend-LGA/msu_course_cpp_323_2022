CC = clang++
FLAGS = -Wall -Werror -std=c++17
PROGS.O = graph.o printing.o main.o
PROGS.CPP = graph.cpp printing.cpp main.cpp
PROGS.HPP = graph.hpp printing.hpp 
RES = main

all: $(PROGS.O)
	$(CC) $(PROGS.O) -o $(RES) $(FLAGS) && ./$(RES)

%.o : %.cpp
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(PROGS.O)

delete: clean
	rm -f $(RES)

clang-format:
	clang-format -i -style=Chromium $(PROGS.CPP) $(PROGS.HPP)

