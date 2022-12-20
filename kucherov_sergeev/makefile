CC = clang++
LDFLAGS = -std=c++17 -Wall -Werror -pthread
CFLAGS = -std=c++17 -Wall -Werror -pthread

SOURCES=main.cpp graph_generator.cpp graph_generation_controller.cpp graph_json_printing.cpp graph_printing.cpp graph.cpp logger.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=run

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
