CC=g++
LINKER=g++
SEARCH_OBJS=../../cpp/search.o ../../cpp/node.o
OBJS=infix_types.o infix_constants.o RemoteInfixSearch.o 
SERVER_OBJ=RemoteInfixSearch_server.o
CLIENT_OBJ=client.o
LINKFLAGS=-g -lthrift -lthriftz -lthriftnb -lboost_system -lpthread -lboost_thread
CFLAGS=-g

PROG=service-cpp
CLIENT=client

compile: $(OBJS) $(SERVER_OBJ)
	$(LINKER) -o $(PROG) $(SEARCH_OBJS) $(OBJS) $(SERVER_OBJ) $(LINKFLAGS) 

client: $(OBJS) $(CLIENT_OBJ)
	$(LINKER) -o $(CLIENT) $(SEARCH_OBJS) $(OBJS) $(CLIENT_OBJ) $(LINKFLAGS) 
	

infix_constants.o: infix_constants.cpp infix_constants.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

infix_types.o: infix_types.cpp infix_types.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

RemoteInfixSearch.o: RemoteInfixSearch.cpp RemoteInfixSearch.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

RemoteInfixSearch_server.o: RemoteInfixSearch_server.cpp RemoteInfixSearch.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<

client.o: client.cpp RemoteInfixSearch.h
	$(CC) $(CFLAGS) $(PREPROC) -c $<


clean:
	rm -f *.o $(PROG)
