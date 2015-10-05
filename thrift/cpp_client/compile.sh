g++ -c *.cpp
g++ -o client *.o ../gen-cpp/*.o -lthrift
