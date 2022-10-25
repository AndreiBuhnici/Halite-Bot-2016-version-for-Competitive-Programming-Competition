CC=g++
CFLAGS=-g -Wall -std=c++11 -c
LFLAGS=-lm

SRCDIR=sources
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=$(wildcard $(SRCDIR)/*.hpp)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

EXECUTABLE="MyBot"

build: $(EXECUTABLE)

run:
	./MyBot

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Linking OK!"

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDES)
	@mkdir -p obj/
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled $< OK!"

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d *.hlt

.PHONY: all clean
