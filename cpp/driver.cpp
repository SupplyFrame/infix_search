#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<list>
#include<stack>
#include<vector>
#include<string.h>
#include<cstdlib>
#include<vector>
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
    cout<<"Size of string: "<<sizeof(string)<<endl;
    cout<<"Size of tp key: "<<sizeof(tree_pos_key_t)<<endl;
    cout<<"Size of tp val: "<<sizeof(tree_pos_val_t)<<endl;
    cout<<"Size of node1: "<<sizeof(node1_t)<<endl;
    cout<<"Size of node2: "<<sizeof(node2_t)<<endl;
    return 1;
  }
  int arg=0;
  string algorithm,dict_file,query_file;
  for(int arg=1;arg<argc;++arg){
    if (strcmp(argv[arg],"--algorithm")==0){
      algorithm = argv[arg+1];
    }else if (strcmp(argv[arg],"--dict")==0){
      dict_file = argv[arg+1];
    }else if (strcmp(argv[arg],"--query")==0){
      query_file = argv[arg+1];
    }
  }
  search_t search;
  bool simple = false;
  search.run(simple,algorithm,dict_file,query_file);
  //while(true);
  return 0;
}
