CC=gcc
CFLAGS=-c -Wall -Wno-unused  
LDFLAGS=-O3
SOURCES=src/dictionary.c src/match.c src/commands.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dictionary

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)
