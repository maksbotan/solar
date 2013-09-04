
SOURCES = main.cpp rendering.cpp bmp_loader.cpp planet.cpp
OBJECTS = $(SOURCES:.cpp=.o)

CXX ?= clang++
CXXFLAGS ?= -Wall -Wextra -g -ggdb
SDL_INCLUDES = $(shell sdl2-config --cflags) $(shell pkg-config SDL2_ttf --cflags)
LIBS = -lGL -lGLU
SDL_LIBS = $(shell sdl2-config --libs) $(shell pkg-config SDL2_ttf --libs)

all: solar

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SDL_INCLUDES) -std=c++11 -c $< -o $@

solar: $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) $(SDL_LIBS) -o $@

clean:
	rm -rf .depend *.o solar

run: solar
	./solar

depend: .depend

.depend: $(SOURCES)
	rm -f .depend
	$(CXX) $(CXXFLAGS) $(SDL_INCLUDES) -MM $^ > .depend

-include .depend

.PHONY: clean
