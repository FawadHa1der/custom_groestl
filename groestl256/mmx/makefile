# Makefile

# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall

# Output executable name
OUTPUT = myprogram

# Source files
SOURCES = hash.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Header files
HEADERS = api.h brg_endian.h brg_types.h hash.h

# Build all
all: $(OUTPUT)

# Link
$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJECTS)

# Compile
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Clean
clean:
	rm -f $(OBJECTS) $(OUTPUT)
