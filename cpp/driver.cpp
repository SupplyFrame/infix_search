#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<list>
#include<stack>
#include<vector>
#include<string.h>
#include<cstdlib>
using namespace std;
#include"node.hpp"
#include"search.hpp"

void test_node(){
  //node_t n1;
  //node2_t n2;
}

int main(int argc,char * argv[]){
  if(argc<2){
    cerr<<"Usage: "<<argv[0]<<" --algoritm <algorithm> --dict <dictionaryfile> --query <queryfile>\n";
		test_node();
    return 1;
  }
  search_t search(argc,argv);
  bool simple = false;
  search.run(simple);
  return 0;
}
