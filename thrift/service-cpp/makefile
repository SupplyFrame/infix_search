CC=g++
LINKER=g++
SEARCH_OBJS=../../cpp/search.o ../../cpp/node.o
OBJS=infix_types.o infix_constants.o RemoteInfixSearch.o RemoteInfixSearch_server.o 
LINKFLAGS=-g -lthrift
CFLAGS=-g

PROG=service-cpp

compile: $(OBJS) 
	$(LINKER) -o $(PROG) $(SEARCH_OBJS) $(OBJS) $(LINKFLAGS) 

infix_constants.o: infix_constants.cpp infix_constants.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

infix_types.o: infix_types.cpp infix_types.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

RemoteInfixSearch.o: RemoteInfixSearch.cpp RemoteInfixSearch.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

RemoteInfixSearch_server.o: RemoteInfixSearch_server.cpp RemoteInfixSearch.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<


clean:
	rm -f *.o $(PROG)
