
SOURCES = main.cpp rendering.cpp bmp_loader.cpp planet.cpp
OBJECTS = $(SOURCES:.cpp=.o)

CXX ?= clang++
CXXFLAGS ?= -Wall -Wextra -g -ggdb
LIBS = -lGL -lGLU -lglut

all: solar

.cpp.o:
	$(CXX) $(CXXFLAGS) -std=c++11 -c $< -o $@

solar: $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf .depend *.o solar

run: solar
	./solar

depend: .depend

.depend: $(SOURCES)
	rm -f .depend
	$(CXX) $(CFLAGS) -MM $^ > .depend

-include .depend

.PHONY: clean
