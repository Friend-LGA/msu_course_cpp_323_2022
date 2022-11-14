TARGET = prog
COMPILER = clang++
COMPILER_FLAGS = -std=c++17 -Werror
LINTER = clang-format
LINTER_OPTIONS = -style=Chromium

SOURCE = $(wildcard *.cpp)
OBJECT = $(patsubst %.cpp, %.o, $(SOURCE))

$(TARGET): $(OBJECT)
	$(COMPILER) $< -o $@ $(COMPILER_FLAGS)

%.o: %.cpp
	$(LINTER) -i $(LINTER_OPTIONS) $<
	$(COMPILER) -c $< -o $@ $(COMPILER_FLAGS)

clean:
	rm *.o $(TARGET)

