CC=clang++

LFLAGS = -std=c++17 -Werror -pthread
CFLAGS = -std=c++17 -Werror -pthread
OBJ = graph.o graph_generator.o graph_generation_controller.o graph_printing.o graph_json_printing.o main.o graph_logger.o

all: prog

prog: $(OBJ)
	${CC} $(OBJ) $(LFLAGS) -o $@

.cpp.o:
	${CC} -g ${CFLAGS} -c $< -o $@

main.o: graph.hpp graph_generator.hpp graph_generation_controller.hpp graph_printing.hpp graph_json_printing.hpp graph_logger.hpp config.hpp

clean:
	rm -rf *.o prog
