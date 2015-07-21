
CC=g++
LINKER=g++
OBJS=search.o node.o driver.o

PROG=../bin/search

compile: $(OBJS)
	$(LINKER) -o $(PROG) $(LINKFLAGS) $(OBJS)

search.o: search.cpp search.hpp
	$(CC) $(CFLAGS) $(PREPROC) -c $<

node.o: node.cpp node.hpp
	$(CC) $(CFLAGS) $(PREPROC) -c $<

driver.o: driver.cpp
	$(CC) $(CFLAGS) $(PREPROC) -c $<

clean:
	rm -f *.o $(PROG)



