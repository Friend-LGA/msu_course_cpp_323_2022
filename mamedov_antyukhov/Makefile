CC = clang++
FLAGS = -Wall -Werror -std=c++17
PROGS.O = graph.o printing.o main.o graph_generator.o
PROGS.CPP = graph.cpp printing.cpp graph_generator.cpp main.cpp
PROGS.HPP = graph.hpp printing.hpp graph_generator.hpp
RES = main
GRAPH = graph.json

all: $(PROGS.O)
	$(CC) $(PROGS.O) -o $(RES) $(FLAGS)

%.o : %.cpp
	$(CC) -c $< $(CFLAGS)

run:
	./$(RES)

clean:
	rm -f $(PROGS.O)

delete: clean
	rm -f $(RES)

erase: delete
	rm -f $(GRAPH)

clang-format:
	clang-format -i -style=Chromium $(PROGS.CPP) $(PROGS.HPP)

