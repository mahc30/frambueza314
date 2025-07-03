# Makefile for building the 'main' project

# Compiler settings - Can change to gcc if required
CC = g++

# Compiler flags
CFLAGS = -Wall -O2 -DDEBUG
CXXFLAGS = -std=c++20 -DDEBUG

# Linker flags
LDFLAGS = -lwiringPi -lwiringPiDev

# Source files
SOURCES = led.cpp DHT.cpp lcd.c main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

# Output executable
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
