CC=clang++

LFLAGS = -std=c++17 -Werror
CFLAGS = -std=c++17 -Werror
OBJ = graph.o graph_generator.o graph_printing.o graph_json_printing.o main.o

all: prog

prog: $(OBJ)
	${CC} $(OBJ) $(LFLAGS) -o $@

.cpp.o:
	${CC} -g ${CFLAGS} -c $< -o $@

main.o: graph.hpp graph_generator.hpp graph_printing.hpp graph_json_printing.hpp

clean:
	rm -rf *.o prog
