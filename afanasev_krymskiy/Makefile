TARGET = prog
COMPILER = clang++
COMPILER_FLAGS = -std=c++17 -Werror -pthread
LINTER = clang-format
LINTER_OPTIONS = -style=Chromium

SOURCE = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCE))

$(TARGET): $(OBJECTS)
	$(COMPILER) $(COMPILER_FLAGS) -o $@ $^

%.o: %.cpp %.hpp
	$(LINTER) -i $(LINTER_OPTIONS) $^
	$(COMPILER) $(COMPILER_FLAGS) -c -o $@ $<

%.o: %.cpp
	$(LINTER) -i $(LINTER_OPTIONS) $<
	$(COMPILER) $(COMPILER_FLAGS) -c -o $@ $<

clean:
	rm *.o $(TARGET)

