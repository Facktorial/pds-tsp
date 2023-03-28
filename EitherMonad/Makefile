all: 
	#gcc code.c -o code file.o -library
	clang++-14 -g -Weverything -Wno-c++98-compat -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wuninitialized -Wno-unused-macros -std=c++20 either_example.cpp -o either_example
	clang++-14 -g -Weverything -Wno-c++98-compat -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wuninitialized -Wno-unused-macros -std=c++20 either_main.cpp -o either_main
clean:
	rm -f either_example either_main
