CC=clang++-14
CFLAGS=-g -Weverything -Wno-c++98-compat -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wuninitialized -Wno-c++98-compat-pedantic -Wno-unused-macros -std=c++20 -mavx2 -mavx -msse2
CSANITIZERS=-fsanitize=address -fsanitize=leak

TARGET_EXTENSION = .out

#: one.cpp
	#$(CC) $(CFLAGS) $(CSANITIZERS) one.cpp -o one

#run:
	#./one

# Default target
all: run

# Compile source file into a binary
%$(TARGET_EXTENSION): %
	$(CC) $(CFLAGS) $(CSANITIZERS) -o $@ $<

# Run the generated binary
run-%: %$(TARGET_EXTENSION)
	./$<

# Clean up generated files
clean:
	rm -f *$(TARGET_EXTENSION)

# Default target dependencies
run: $(basename $(wildcard *))
