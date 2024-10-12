# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++11

# Target executable
TARGET = QuantumCircuit

# Source files
SRCS = QuantumCircuit.cpp

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build (removes object files and executable)
clean:
	rm -f $(OBJS) $(TARGET)
