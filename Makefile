
SOURCES = main.c rendering.c bmp_loader.c
OBJECTS = $(SOURCES:.c=.o)

CC = clang
CFLAGS ?= -Wall -Wextra -g -ggdb
LIBS = -lGL -lGLU -lglut

all: solar

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

solar: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf *.o solar

run: solar
	./solar

.PHONY: clean
