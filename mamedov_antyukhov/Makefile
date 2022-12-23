CC = clang++
FLAGS = -std=c++17 -Werror -pthread

PROGS.O = graph.o logger.o graph_printing.o graph_generator.o graph_json_printing.o graph_generation_controller.o main.o
PROGS.CPP = graph.cpp logger.cpp graph_printing.cpp graph_generator.cpp graph_json_printing.cpp graph_generation_controller.cpp main.cpp
PROGS.HPP = graph.hpp logger.hpp graph_printing.hpp graph_generator.hpp graph_json_printing.hpp config.hpp graph_generation_controller.hpp
INTERFACES = interfaces/i_edge.hpp interfaces/i_graph.hpp interfaces/i_vertex.hpp interfaces/i_worker.hpp  
RES = main
GRAPH = graph.json
TEMP = ./temp

all: $(PROGS.O)
	$(CC) $(PROGS.O) -o $(RES) $(FLAGS) -std=c++17

%.o : %.cpp
	$(CC) -c $< $(FLAGS)

run:
	./$(RES)

clean:
	rm -f $(PROGS.O)

delete: clean
	rm -rf $(TEMP)

erase: delete
	rm -f $(RES)

clang-format:
	clang-format -i -style=Chromium $(PROGS.HPP) $(PROGS.CPP) $(INTERFACES)
