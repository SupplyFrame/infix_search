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


int search_t::node_count;
search_t::search_t(int argc,char * argv[]){
  head=NULL;
  traverse=head;
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

void search_t::run(bool simple){
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
}

void search_t::add_to_lookup(char parent_val,node_t * child_node){
  bool debug_add = false;
  if(nlmm.find(parent_val)!=nlmm.end()){
    //nlm = nlmm[parent_val];
    if(debug_add) cerr<<"Reusing nlm with key "<<parent_val<<endl;
    if(nlmm[parent_val].find(child_node->get_val())!=nlmm[parent_val].end()){
      nlmm[parent_val][child_node->get_val()].push_back(child_node);
      if(debug_add) cerr<<"Reusing nl with key "<<child_node->get_val()<<endl;
    }else{
      node_list_t nl;
      nl.push_back(child_node);
      nlmm[parent_val][child_node->get_val()] = nl;
    }
  }else{
    node_list_t nl;
    nl.push_back(child_node);
    node_list_map_t nlm;
    nlm[child_node->get_val()] = nl;
    nlmm[parent_val] = nlm;
  }  
  // now append to this list
  if(debug_add) cerr<<"Connecting child node "<<child_node->get_val()<<" to "<<parent_val<<endl;
}

void search_t::add(string word){
  bool debug = false;
  if(debug)cerr<<"\nAdding "<<word<<endl;
  int len = word.length();
  if(head==NULL){
    head = new node_t();
    head->set_val('*');
  }
  traverse = head;
  node_t * previous = head;
  int i=0;
  while(i<len){
    node_t * child = NULL;
    node_t * parent = traverse; 
    if(debug) cerr<<"Traverse is now at addr "<<traverse<<endl;
    if(traverse->get_tail().length()){
      int tail_index = 0;
      string traverse_tail = traverse->get_tail();
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
        if(debug)cerr<<"Clearing out node tail with address "<<traverse<<endl;
        traverse->set_tail("");
        traverse->set_is_leaf(false);
        // this loop builds a path up to before the mismatch
        for(int j=i_begin;j<i;++j){
          parent = traverse;
          child = new node_t();
          child->set_val(word[j]);
          traverse->set_child(word[j],child);
          if(debug)cerr<<"Forging new path. Added a new child to "<<traverse<<" with key "<<word[j]<<endl;
          add_to_lookup(parent->get_val(),child); 
          traverse = child;
          //if(debug)cerr<<"traverse for "<<parent->get_val()<<" to "<<word[j]<<" has addr "<<traverse<<endl;
          traverse->set_parent(parent);
        }
        //traverse = parent; // backtrack one
        if(tail_index<traverse_tail.length())
        traverse->set_tail(traverse_tail.substr(tail_index));
        traverse->set_is_leaf(true);
        //i = tail_index;
        if(debug)cerr<<"After breaking up tail, at position "<<i<<" with suffix "<<traverse->get_tail()<<" into address "<<traverse<<endl;
      }
    }
    if (!traverse->child_exists(word[i])){
    //if (traverse->child_map.find(word[i])==traverse->child_map.end()){
      child = new node_t();
      child->set_val(word[i]);
      if(word.length()>i+1) child->set_tail(word.substr(i+1));
      traverse->set_child(word[i],child);
      if(debug)cerr<<"No child "<<child<<" with key "<<word[i]<<". Added a new child with key "<<word[i]<<" and suffix "<<child->get_tail()<<" into "<<traverse<<endl;
      //GKC 2015-07-19
      add_to_lookup(traverse->get_val(),child); 
    }else{
      child = traverse->get_child(word[i]); 
      if(debug)cerr<<"Reusing child with key "<<word[i]<<" at "<<child<<endl;
    }
    traverse = child;
    traverse->set_parent(parent);
    ++i;
  }
  if(debug)cerr<<"Traverse is at child with addr "<<traverse<<endl;
  traverse->set_is_leaf(true);
}

void search_t::traverse_up(node_t * node,stack<char> & up_stack){
  if(node==head) return;
  else{
    up_stack.push(node->get_val());
    traverse_up(node->get_parent(),up_stack);
  }
}

void search_t::traverse_down_util(node_t * node, char path[],int pathLen,list<string> & suffixes){
  bool debug = false;
  path[pathLen] = node->get_val();
  if(debug ) cerr<<"parent-node at index "<<pathLen<<" is "<<node->get_parent()->get_val()<<node->get_val()<<" with address "<<node<<" and tail "<<node->get_tail()<<endl;
  ++pathLen;
  path[pathLen]='\0';
  //if(debug) cerr<<"Node at addr "<<node<<" has tail "<<node->get_tail()<<endl;
  if (node->get_is_leaf() ){ 
    string pathstr = string(path);
    ostringstream oss;
    oss<<pathstr<<"*"<<node->get_tail();
    suffixes.push_back(oss.str());
    //suffixes.push_back(pathstr);
  }
  node_list_t node_list = node->get_children();
  for(node_list_t::iterator it = node_list.begin();it!=node_list.end();it++){

  //for(node_map_t::iterator it = node->child_map.begin();it!=node->child_map.end();it++){
    node_t * node = *it;
    //++pathLen;
    traverse_down_util(node,path,pathLen,suffixes);
  } 
}

void search_t::traverse_down(node_t * node,list<string> & suffixes,string orig_key){
  char path[100];
  int key_len = orig_key.length();
  for(int i=0;i<key_len;++i) path[i] = orig_key[i];
  traverse_down_util(node,path,key_len-1,suffixes);
}


bool search_t::find_path_util(node_t * node,string sub_key,node_t * start,string orig_key,bool & exact_match){
  bool debug = false;
  //if(debug)cerr<<"Substring is "<<sub_key<<" start,end is "<<start->get_val()<<","<<node->get_val()<<endl;
  if(sub_key.length()<2 || node==NULL){
    if(debug)cerr<<"Found!\n";  
    stack<char> up_stack;
    traverse_up(start->get_parent(),up_stack);
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
    }else{ 
      // case where tail was the suffix. Add to suffixes list
      ostringstream oss;
      oss<<orig_key<<"*"<<sub_key;
      string newsuffix = oss.str();
      if(debug) cerr<<"Adding "<<newsuffix<<" to suffix list\n";
      suffixes.push_back(newsuffix);
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
  if (node->get_val()==sub_key[0] && node->child_exists(sub_key[1])){
  //if (node->get_val()==sub_key[0] && node->child_map.find(sub_key[1])!=node->child_map.end()){
    if(debug)cerr<<"Found a match from "<<sub_key[0]<<" to "<<sub_key[1]<<" recursing on shorter str "<<sub_key.substr(1)<<" on node "<<node->get_child(sub_key[1]) <<endl;
    exact_match = false;
    find_path_util(node->get_child(sub_key[1]),sub_key.substr(1),start,orig_key,exact_match);
    //}else{
     // for(node_map_t::iterator it=node->child_map.begin();it!=node->child_map.end();it++){
      //  if(debug)cerr<<" not found children from "<<node->get_val()<<" are "<<it->first<<endl;
      //}
    //}
  }else{
    // check if remainder of sub_key is a prefix of the tail
    string remain = sub_key.substr(1);
    int i=0,j=0;
    while(i<remain.length() && j<node->get_tail().length()){
      if (remain[i]!=node->get_tail()[j]) break;
      ++i;++j;
    }
    if(i==remain.length()){
      if(debug)cerr<<"Subkey is a prefix of tail, accepting match. Node addr is "<<node<<"\n";
      if(debug)cerr<<"Found a match between tail "<<node->get_tail()<<" and "<<sub_key.substr(1)<<endl;
      exact_match = false;
      string newstr="";
      if(i==node->get_tail().length()){
        if(debug)cerr<<"Exact match found. Calling find_path_util on null string\n";
        exact_match = true;
        //find_path_util(NULL,"",start,orig_key);
      }else{
        newstr=node->get_tail().substr(i);
        if(debug)cerr<<"Calling find_path_util on string "<<node->get_tail().substr(i)<<"\n";
        //find_path_util(NULL,node->get_tail().substr(i),start,orig_key);
      }
      find_path_util(NULL,newstr,start,orig_key,exact_match);
    }else{
      //if(debug)cerr<<"Not matched with sub key of "<<sub_key<<"\n";
      return false;
    }
  }
  return false;
}

void search_t::find_path(string key){
  bool debug = false;
  int keylen = key.length();
  if(nlmm.find(key[0])==nlmm.end()){
    cerr<<"Cannot start a path!";
    return;
  }
  else{
    //node_list_map_t nlm = nlmm[key[0]];
    if (nlmm[key[0]].find(key[1])!=nlmm[key[0]].end()){
      node_list_t nl = nlmm[key[0]][key[1]];
      bool exact_match = false;
      for(node_list_t::iterator it = nl.begin();it!=nl.end();it++){
        node_t * cur = *it;
        if(debug)cerr<<"Calling path util from start for pair "<<key[0]<<","<<key[1]<<"("<<cur<<")\n";
        find_path_util(cur,key.substr(1),cur->get_parent(),key,exact_match);
        if(debug)cerr<<"In node list search, exact match "<<exact_match<<endl;
        if(exact_match){
          if(debug)cerr<<"Exact match found. Breaking early from lookup table search\n";
           break;
        }
      }
    }
  }
}


void search_t::read_query(const char * query_file,vector<string> & keys){
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
void search_t::read_input(const char * dict_file){
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

void search_t::print_lookup(){
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



void search_t::find_in_file(string key,const char * dict_file){
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
