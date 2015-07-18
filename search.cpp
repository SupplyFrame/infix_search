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

int node_count = 0;
string algorithm,dict_file,query_file;

struct node_t;
typedef map<char,node_t *> node_map_t;
struct node_t{
  char val;
  bool is_leaf;
  node_map_t child_map;
  node_t * parent;
  string tail;
  node_t(){
    val = '*';
    parent = NULL;
    tail="";
    is_leaf = false;
    ++node_count;
  }
};

typedef list<node_t *> node_list_t;
typedef map<char,node_list_t> node_list_map_t;
typedef map<char,node_list_map_t> node_list_map_map_t;

node_t * head = NULL;
node_t * traverse = head;
node_list_map_map_t nlmm;

void add_to_lookup(char parent_val,node_t * child_node){
  bool debug_add = false;
  if(nlmm.find(parent_val)!=nlmm.end()){
    //nlm = nlmm[parent_val];
    if(debug_add) cerr<<"Reusing nlm with key "<<parent_val<<endl;
    if(nlmm[parent_val].find(child_node->val)!=nlmm[parent_val].end()){
      nlmm[parent_val][child_node->val].push_back(child_node);
      if(debug_add) cerr<<"Reusing nl with key "<<child_node->val<<endl;
    }else{
      node_list_t nl;
      nl.push_back(child_node);
      nlmm[parent_val][child_node->val] = nl;
    }
  }else{
    node_list_t nl;
    nl.push_back(child_node);
    node_list_map_t nlm;
    nlm[child_node->val] = nl;
    nlmm[parent_val] = nlm;
  }  
  // now append to this list
  if(debug_add) cerr<<"Connecting child node "<<child_node->val<<" to "<<parent_val<<endl;
}

void add(string word){
  bool debug = true;
  if(debug)cerr<<"\nAdding "<<word<<endl;
  int len = word.length();
  if(head==NULL){
    head = new node_t();
    head->val = '*';
  }
  traverse = head;
  node_t * previous = head;
  int i=0;
  while(i<len){
    node_t * child = NULL;
    node_t * parent = traverse; 
    if(debug) cerr<<"Traverse is now at addr "<<traverse<<endl;
    if(traverse->tail.length()){
      int tail_index = 0;
      string traverse_tail = traverse->tail;
      if(debug)cerr<<"Traverse tail exists and is "<<traverse_tail<<". Comparing remainder of word to tail. "<<endl;
      // count matches. If matches are equal then no need to branch off
      int i_begin = i;
      while(tail_index<traverse_tail.length() && i < word.length() &&
      traverse_tail[tail_index]==word[i]){
        if(debug) cerr<<"Comparing key "<<word[i]<<" to tail "<<traverse_tail[tail_index]<<" for i "<<i<<endl;
        ++i;
        ++tail_index;
      }
      if(i==word.length() && tail_index==traverse_tail.length()){
        if(debug) cerr<<"Exact match to tail. Don't split.\n";
        break;
      }else{
        if(debug) cerr<<"Not a match to tail. Mismatch is at tail_index "<<tail_index<<" and i "<<i<<".\n";
        // clear out the tail of the parent
        cerr<<"Clearing out node tail with address "<<traverse<<endl;
        traverse->tail="";
        traverse->is_leaf = false;
        // this loop builds a path up to before the mismatch
        for(int j=i_begin;j<i;++j){
          parent = traverse;
          child = new node_t();
          child->val = word[j];
          traverse->child_map[word[j]] = child;
          if(debug)cerr<<"Forging new path. Added a new child to "<<traverse<<" with key "<<word[j]<<endl;
          add_to_lookup(parent->val,child); 
          traverse = child;
          //if(debug)cerr<<"traverse for "<<parent->val<<" to "<<word[j]<<" has addr "<<traverse<<endl;
          traverse->parent = parent;
        }
        //traverse = parent; // backtrack one
        if(tail_index<traverse_tail.length())
        traverse->tail = traverse_tail.substr(tail_index);
        traverse->is_leaf = true;
        //i = tail_index;
        if(debug)cerr<<"After breaking up tail, at position "<<i<<" with suffix "<<traverse->tail<<" into address "<<traverse<<endl;
      }
    }
    if (traverse->child_map.find(word[i])==traverse->child_map.end()){
      child = new node_t();
      child->val = word[i];
      if(word.length()>i+1) child->tail = word.substr(i+1);
      traverse->child_map[word[i]] = child;
      if(debug)cerr<<"No child "<<child<<" with key "<<word[i]<<". Added a new child with key "<<word[i]<<" and suffix "<<child->tail<<" into "<<traverse<<endl;
      add_to_lookup(parent->val,child); 
    }else{
      child = traverse->child_map[word[i]]; 
      if(debug)cerr<<"Reusing child with key "<<word[i]<<" at "<<child<<endl;
    }
    traverse = child;
    traverse->parent = parent;
    ++i;
  }
  if(debug)cerr<<"Traverse is at child with addr "<<traverse<<endl;
  traverse->is_leaf = true;
}

void traverse_up(node_t * node,stack<char> & up_stack){
  if(node==head) return;
  else{
    up_stack.push(node->val);
    traverse_up(node->parent,up_stack);
  }
}

void traverse_down_util(node_t * node, char path[],int pathLen,list<string> & suffixes){
  bool debug = true;
  path[pathLen] = node->val;
  if(debug ) cerr<<"parent-node at index "<<pathLen<<" is "<<node->parent->val<<node->val<<" with address "<<node<<" and tail "<<node->tail<<endl;
  ++pathLen;
  path[pathLen]='\0';
  //if(debug) cerr<<"Node at addr "<<node<<" has tail "<<node->tail<<endl;
  if (node->is_leaf ){ 
    string pathstr = string(path);
    ostringstream oss;
    oss<<pathstr<<"*"<<node->tail;
    suffixes.push_back(oss.str());
    //suffixes.push_back(pathstr);
  }
    for(node_map_t::iterator it = node->child_map.begin();it!=node->child_map.end();it++){
      node_t * node = it->second;
      //++pathLen;
      traverse_down_util(node,path,pathLen,suffixes);
    } 
  
}

void traverse_down(node_t * node,list<string> & suffixes,string orig_key){
  char path[100];
  int key_len = orig_key.length();
  for(int i=0;i<key_len;++i) path[i] = orig_key[i];
  traverse_down_util(node,path,key_len-1,suffixes);
}


bool find_path_util(node_t * node,string sub_key,node_t * start,string orig_key){
  bool debug = true;
  cerr<<"Substring is "<<sub_key<<" start is "<<start<<endl;
  if(sub_key.length()<2 || node==NULL){
    if(debug)cerr<<"Found!\n";  
    stack<char> up_stack;
    traverse_up(start->parent,up_stack);
    char buf[100];
    int p=0;
    while(!up_stack.empty()){
     char c = up_stack.top();
     up_stack.pop();
     //cout<<c;
     buf[p++]=c;
    }
    buf[p] = '\0';
    //cout<<endl;
    string prefix = string(buf);
    if(debug)cerr<<"PREFIX: "<<prefix<<endl;
    list<string> suffixes;
    if(node!=NULL){
      traverse_down(node,suffixes,orig_key);
    }
    if(suffixes.size()>0){
      for(list<string>::iterator it = suffixes.begin();it!=suffixes.end();it++){
        string suffix = *it;
        if(debug)cerr<<"SUFFIX: "<<suffix<<endl;
        cout<<"MATCH TO with suffix "<<prefix<<"*"<<suffix<<endl;
      }
    }else{
        cout<<"MATCH TO with no suffix "<<prefix<<"*"<<orig_key<<endl;
    }
    return true;
  }
  if (node->val==sub_key[0] && node->child_map.find(sub_key[1])!=node->child_map.end()){
    if(debug)cerr<<"Found a match from "<<sub_key[0]<<" to "<<sub_key[1]<<endl;
    find_path_util(node->child_map[sub_key[1]],sub_key.substr(1),start,orig_key);
  }else if(node->tail.compare(sub_key.substr(1))==0){
    if(debug)cerr<<"Found a match between tail "<<node->tail<<" and "<<sub_key.substr(1)<<endl;
    find_path_util(NULL,sub_key.substr(1),start,orig_key);
  }else{
    if(debug)cerr<<"Not matched with sub key of "<<sub_key<<"\n";
    return false;
  }
  return false;
}

void find_path(string key){
  bool debug = true;
  int keylen = key.length();
  if(nlmm.find(key[0])==nlmm.end()){
    cerr<<"Cannot start a path!";
    return;
  }
  else{
    //node_list_map_t nlm = nlmm[key[0]];
    if (nlmm[key[0]].find(key[1])!=nlmm[key[0]].end()){
      node_list_t nl = nlmm[key[0]][key[1]];
      for(node_list_t::iterator it = nl.begin();it!=nl.end();it++){
        if(debug)cerr<<"Calling path util from start\n";
        node_t * cur = *it;
        find_path_util(cur,key.substr(1),cur->parent,key);
      }
    }
  }
}


void read_query(const char * query_file,vector<string> & keys){
  ifstream ifs(query_file);
  if(!ifs.is_open()){
    cerr<<"Can't open "<<query_file<<"!\n";
    exit(1);
  }
  string line;
  int i=0;
  while(getline(ifs,line)){
    keys.push_back(line);
    cerr<<"Read in query "<<line<<endl;
    ++i;
  }
  ifs.close();
}
void read_input(const char * dict_file){
  ifstream ifs(dict_file);
  if(!ifs.is_open()){
    cerr<<"Can't open "<<dict_file<<"!\n";
    exit(1);
  }
  string line;
  int i=0;
  double start = clock();
  while(getline(ifs,line)){
    add(line);
    if(i%100000==0)cerr<<"Current node count at line "<<i<<": "<<node_count<<endl;
    ++i;
  }
  cerr<<"TIME TO LOAD AND BUILD TRIE: "<<(clock()-start)/CLOCKS_PER_SEC<<endl;
  ifs.close();
}

void print_lookup(){
  cerr<<"Debugging lookup table\n";
  for(node_list_map_map_t::iterator it1 = nlmm.begin();
  it1!=nlmm.end();it1++){
    cerr<<"parent: "<<it1->first<<endl;
    node_list_map_t nlm = it1->second;
    for(node_list_map_t::iterator it2 = nlm.begin();
    it2!=nlm.end();it2++){
      cerr<<" child: "<<it2->first<<endl;
      node_list_t nl = it2->second; 
      for(node_list_t::iterator it3 = nl.begin();
      it3!=nl.end();it3++){
        cerr<<"  Address: "<<*it3<<endl;
      }
    }
  }
}


void parse_args(int argc,char * argv[]){
  int arg=0;
  
  for(int arg=1;arg<argc;++arg){
    if (strcmp(argv[arg],"--algorithm")==0){
      algorithm = argv[arg+1];
    }else if (strcmp(argv[arg],"--dict")==0){
      dict_file = argv[arg+1];
    }else if (strcmp(argv[arg],"--query")==0){
      query_file = argv[arg+1];
    }
  }
  
}

void find_in_file(string key,const char * dict_file){
  ifstream ifs(dict_file);
  if(!ifs.is_open()){
    cerr<<"Can't open "<<dict_file<<"!\n";
    exit(1);
  }
  string line;
  double start = clock();
  bool found = false;
  while(!found && getline(ifs,line)){
    string::size_type n = line.find(key);
    if(n!=string::npos) cout<<line<<endl;
  }
  //cerr<<"TIME TO SEAR: "<<(clock()-start)/CLOCKS_PER_SEC<<endl;
  ifs.close();

}

int main(int argc,char * argv[]){
  if(argc<2){
    cerr<<"Usage: "<<argv[0]<<" --algoritm <algorithm> --dict <dictionaryfile> --query <queryfile>\n";
    //cerr<<"Size of node: "<<sizeof(node_t)<<endl;
    return 1;
  }
  parse_args(argc,argv);
  bool simple = false;
  if(simple){
    string words[]={"baby","bad","bank","box","dad","dance","abdade","abdadoma","dadomb"};
		for(int i=0;i<9;++i){
      add(words[i]);
    }
    bool debug_lookup = false;
    if(debug_lookup){
      print_lookup();
    }
    string keys[]={"ade"};
    for(int i=0;i<1;++i){
      //bool res = find(keys[i]);
      //cerr<<"Result for key "<<keys[i]<<":"<<res<<endl;
      find_path(keys[i]);
    }
  }else{
    if(algorithm.compare("trie")==0){
      read_input(dict_file.data());
      //string keys[]={"DCRK","DCRK500","##BP","DNA"};
      vector<string> keys;
      read_query(query_file.data(),keys);
      double start = clock();
      for(int i=0;i<keys.size();++i){
        //bool res = find(keys[i]);
        cout<<"Result for key "<<keys[i]<<":"<<endl;
        find_path(keys[i]);
      }
      cerr<<"TIME TO EXECUTE QUERYS: "<<(clock()-start)/CLOCKS_PER_SEC<<endl;
    }else if(algorithm.compare("linear")==0){
      vector<string> keys;
      read_query(query_file.data(),keys);
      double start = clock();
      for(int i=0;i<keys.size();++i){
        //bool res = find(keys[i]);
        cout<<"Result for key "<<keys[i]<<":"<<endl;
        find_in_file(keys[i],dict_file.data());
      }
      cerr<<"TIME TO EXECUTE QUERYS: "<<(clock()-start)/CLOCKS_PER_SEC<<endl;
    }
  }
  return 0;
}
