all:
	clear	
	clang-format -i -style=Chromium *.hpp
	clang-format -i -style=Chromium *.cpp
	clang++ main.cpp -o run -std=c++17
	./run

