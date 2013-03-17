CC=gcc
CFLAGS=-c -Wall -Wno-unused
LDFLAGS=
SOURCES=dictionary.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dictionary

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)
