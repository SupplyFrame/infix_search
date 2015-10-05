g++ -c *.cpp
g++ -o server *.o ../gen-cpp/*.o ../../cpp/search.o ../../cpp/node.o -lthrift
