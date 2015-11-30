
CC=g++
LINKER=g++
OBJS=build.o search.o
LINKFLAGS=-g
CFLAGS=-g -I ../src
PROGRAMS=build search 
compile: $(PROGRAMS)

build: build.cpp ../src/dawgdic/dawg-builder.h
	$(LINKER) -o build $(CFLAGS) build.cpp

search: search.cpp
	$(LINKER) -o search $(CFLAGS) search.cpp

clean:
	rm -f $(PROGRAMS)
