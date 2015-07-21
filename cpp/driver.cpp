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
  node_t n1;
  node2_t n2;
  char buf[50];
  //int len = n2.get_token(node2_t::TOKEN_TAIL,buf);
  //char t[10]="gary";
  //n2.set_token(node2_t::TOKEN_TAIL,t);
  //n2.set_token(node2_t::TOKEN_SIBLING_PTR,t);
  //n2.set_token(node2_t::TOKEN_VAL,t);
  node2_t * nptr = new node2_t();
  uintptr_t iptr = reinterpret_cast<uintptr_t>(nptr);
  cerr<<"Ptr is addr "<<nptr<<" of size "<<sizeof(nptr)<<endl;
  cerr<<"iPtr is  "<<iptr<<" of size "<<sizeof(iptr)<<endl;
  char * chptr = reinterpret_cast<char *>(iptr);
  char * t = new char[8];
  for(int i=0;i<8;++i){
    t[i] = chptr[i];
  }
  node2_t * nptr2 = reinterpret_cast<node2_t *>(t);
  cerr<<"(incorrect) Casted ptr is addr "<<nptr2<<" of size "<<sizeof(nptr2)<<endl;
  nptr2 = reinterpret_cast<node2_t *>(chptr);
  cerr<<"(correct) Casted ptr is addr "<<nptr2<<" of size "<<sizeof(nptr2)<<endl;
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
