# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++11

# Target executable
TARGET = QuantumCircuit

# Source files
SRCS = main.cpp circuit.cpp matrix.cpp component.cpp input_handler.cpp  # List all your .cpp files here

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile each .cpp file into .o object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up (remove object files and the executable)
clean:
	rm -f $(OBJS) $(TARGET)