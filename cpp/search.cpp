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
      index_trie(head,NULL);
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
  string key="AA";
  key[0] = parent_val;
  key[1] = child_node->get_val();
  nvm[key].push_back(child_node);
  if(debug_add) cerr<<"Connecting child node "<<child_node->get_val()<<" to "<<parent_val<<endl;
}

void search_t::add(string word){
  bool debug = true;
  word = "*"+word;
  if(debug)cerr<<"\nAdding "<<word<<endl;
  int len = word.length();
  if(head==NULL){
    head = node_t::make_node(node_type);
    head->set_val('*');
    head->set_tail("*");
  }
  traverse = head;
  node_t * previous = head;
  bool single_char = false;
  int i=0;
  while(i<len){
    if(debug) cerr<<"Traverse is now at addr "<<traverse<<endl;
    node_t * parent = traverse; 
    node_t * child = NULL;
    if(traverse->get_tail().length()!=0){
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
        if(debug) cerr<<"Not a match to tail. Mismatch is at tail_index "<<tail_index<<" and i "<<i<<". Proposed tail for traverse is "<<traverse_tail.substr(0,tail_index)<<" and child tail is "<<traverse_tail.substr(tail_index)<<"\n";
        // clear out the tail of the parent
        if(single_char){
        }else{
          // check if there is a child with a key of the current letter
          parent = traverse;
          if(traverse->get_child(word[i])!=NULL){
            child = traverse->get_child(word[i]);
            if(debug)cerr<<"Found a child to follow with key "<<child->get_val()<<endl;
          }else{
            if(tail_index<traverse_tail.length()){
              if(debug)cerr<<"Forking two children"<<endl;
              traverse->set_tail(traverse_tail.substr(0,tail_index));
              node_t * old_fork = node_t::make_node(node_type);
              // copy this node's leaf status to the child
              old_fork->set_is_leaf(traverse->get_is_leaf());
              // copy this node's substring over
              old_fork->set_val(traverse_tail[tail_index]);
              old_fork->set_tail(traverse_tail.substr(tail_index));
              old_fork->set_parent(traverse);
              if(debug)cerr<<"Old fork has parent "<<old_fork->get_parent()<<endl;
              // move this node's children to old_fork
              node_list_t nl = traverse->get_children();
              if(nl.size()){
                if(debug) cerr<<"There are children to move\n";
                for(node_list_t::iterator it = nl.begin();it!=nl.end();it++){
                  node_t * grandchild = *it; 
                  if(debug)cerr<<"Setting child "<<grandchild->get_val()<<" with tail "<<grandchild->get_tail()<<endl;
                  old_fork->set_child(grandchild->get_val(),grandchild);
                  grandchild->set_parent(old_fork);
                }
                traverse->kill_children();
              }
              if(debug)cerr<<"Old fork "<<old_fork<<" will be indexed with char "<<traverse_tail[tail_index]<<" with tail "<<old_fork->get_tail()<<endl;
              // this node was forked so cannot be a leaf.
              traverse->set_is_leaf(false);
              if(debug)cerr<<"Leaf status "<<traverse->get_is_leaf()<<endl;
              // add the old fork to the current node
              traverse->set_child(traverse_tail[tail_index],old_fork);
            }else{
              if(debug)cerr<<"Forking only one child\n";
            }
            // create a new fork the collects remainder of the key
            child = node_t::make_node(node_type);
            child->set_val(word[i]);
            child->set_tail(word.substr(i));
            // the new fork collects the remainder of the key so is a leaf
            child->set_is_leaf(true);
            if(debug)cerr<<"New fork 2 "<<child<<" will be indexed with char "<<word[i]<<" with tail "<<child->get_tail()<<endl;
            // add the new fork to the current node
            traverse->set_child(word[i],child);
          }
        }
      }
    }
    if(single_char){
      if (!traverse->child_exists(word[i])){
      //if (traverse->child_map.find(word[i])==traverse->child_map.end()){
        child = node_t::make_node(node_type);
        //child = new node_t();
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
      ++i;
    }
    traverse = child;
    traverse->set_parent(parent);
  }
  if(debug)cerr<<"Traverse is at child with addr "<<traverse<<endl;
  traverse->set_is_leaf(true);
}

void search_t::traverse_up(tree_pos_val_t tpv,stack<char> & up_stack){
  bool debug = true;
  node_t * node = tpv.first;
  int charindex = tpv.second;
  if(node==head) return;
  else{
    int i=charindex;
    string tail = node->get_tail();
    if(debug)cerr<<"traverse up tail is "<<tail<<endl;
    while(i>=0){
      up_stack.push(tail[i]);
      --i;
    }
    tpv.first = node->get_parent();
    tpv.second = tpv.first->get_tail().length()-1;
    traverse_up(tpv,up_stack);
  }
}

void search_t::traverse_down_util(node_t * node,int tailindex, char path[],int pathLen,list<string> & suffixes){
  bool debug = true;
  string tail = node->get_tail();
  cerr<<"TRAVERSEDOWN: tail is "<<tail<<" tailindex "<<tailindex<<" path len "<<pathLen<<endl;
  int i = tailindex;
  while(i<tail.length()){
    path[pathLen++] = tail[i]; 
    ++i;
  }
  //path[pathLen] = node->get_val();
  if(debug ) cerr<<"parent-node at index "<<pathLen<<" is "<<node->get_parent()->get_tail()<<"("<<node->get_parent() <<"),"<<node->get_tail()<<endl;
  //++pathLen;
  //if(debug) cerr<<"Node at addr "<<node<<" has tail "<<node->get_tail()<<endl;
  if (node->get_is_leaf() ){ 
    path[pathLen]='\0';
    string pathstr = string(path);
    if(debug) cerr<<"Path str is now "<<pathstr<<endl;
    suffixes.push_back(pathstr);
  }
  node_list_t node_list = node->get_children();
  for(node_list_t::iterator it = node_list.begin();it!=node_list.end();it++){

  //for(node_map_t::iterator it = node->child_map.begin();it!=node->child_map.end();it++){
    node_t * node = *it;
    //++pathLen;
    if(debug)cerr<<"TRAVERSING DOWN ONE GENERATION\n";
    traverse_down_util(node,0,path,pathLen,suffixes);
  } 
}

void search_t::traverse_down(node_t * node,list<string> & suffixes,string orig_key){
  char path[100];
  int key_len = orig_key.length();
  for(int i=0;i<key_len;++i) path[i] = orig_key[i];
  //traverse_down_util(node,path,key_len-1,suffixes);
}


bool search_t::find_path_util(node_t * node,int tailindex,string sub_key,tree_pos_val_t start,string orig_key,bool & exact_match){
  bool debug = true;
  //if(debug)cerr<<"Substring is "<<sub_key<<" start,end is "<<start->get_val()<<","<<node->get_val()<<endl;
  if(sub_key.length()<1 || node==NULL){
    if(debug)cerr<<"Found with less than one characters!\n";  
    stack<char> up_stack;
    --start.second;
    traverse_up(start,up_stack);
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
      char path[100];
      int key_len = orig_key.length();
      for(int i=0;i<key_len;++i) path[i] = orig_key[i];
      traverse_down_util(node,tailindex,path,key_len,suffixes);
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
    if(prefix.length()){
      if(debug) cerr<<"Prefix exists so not an exact match\n";
      exact_match = false;
    }
    return true;
  }
  string node_tail = node->get_tail();
  int node_tail_len = node_tail.length();
  int key_len = sub_key.length();
  int keyindex = 0;
  // advance the index for both the tail and the key while there are matches
  while(tailindex<node_tail_len && keyindex<key_len 
  && node_tail[tailindex]==sub_key[keyindex]){
    if(debug)cerr<<"Comparing tail "<<node_tail[tailindex]<<" to key "<<sub_key[keyindex]<<endl;
    ++tailindex;
    ++keyindex;
  }
  if(debug) cerr<<"Tail and key index are "<<tailindex<<","<<keyindex<<endl;
  if(keyindex==key_len){
    exact_match = node->get_is_leaf() && tailindex==node_tail_len;
    if(exact_match){
      if(debug)cerr<<"Exact match is found!\n";
      find_path_util(NULL,tailindex,"",start,orig_key,exact_match);
    }else{
      if(debug)cerr<<"Suffixes remain!\n";
      find_path_util(node,tailindex,"",start,orig_key,exact_match);
    }
  }else{
    if(tailindex<node_tail_len){
      if(debug)cerr<<"Match could not be found at all between key "<<sub_key<<" and tail "<<node_tail<<" Returning false!\n";
      return false;
    }else{
      string newstr=sub_key.substr(keyindex);
      if(debug)cerr<<"Match could not be found so far between key "<<sub_key<<" and tail "<<node_tail<<" Try to recurse on subkey "<<newstr<<"!\n";
      node_t * child = node->get_child(newstr[0]);
      if(child==NULL){
        if(debug) cerr<<"No child with value "<<newstr[0]<<". Aborting search\n";
        return false;
      }else{
        if(debug)cerr<<" Recursing on subkey "<<newstr<<"!\n";
        find_path_util(child,0,newstr,start,orig_key,exact_match);
      }
    }
  }
  return false;
}

void search_t::find_path(string key){
  bool debug = true;
  int keylen = key.length();
  tree_pos_key_t tp_key;
  tp_key.first = key[0];
  tp_key.second = key[1];
  if(tpm.find(tp_key)==tpm.end()){
    cerr<<"Cannot start a path with key "<<key[0]<<" and "<<key[1]<<"!";
    return;
  }
  else{
    tree_pos_val_vector_t nl = tpm[tp_key];
    bool exact_match = false;
    if(debug)cerr<<"Node vector is of length: "<<nl.size()<<endl;
    for(tree_pos_val_vector_t::iterator it = nl.begin();it!=nl.end();it++){
      tree_pos_val_t tpv = *it;
      node_t * cur = tpv.first;
      int tailindex = (int)tpv.second;
      if(debug)cerr<<"Calling path util from start for pair "<<tp_key.first<<","<<tp_key.second<<"("<<cur<<","<<tailindex<<")\n";
      find_path_util(cur,tailindex,key.substr(0),tpv,key,exact_match);
      //if(debug)cerr<<"In node list search, exact match "<<exact_match<<endl;
      if(exact_match){
        if(debug)cerr<<"Exact match found. Breaking early from lookup table search\n";
        break;
      }
    }
  }
}

void search_t::index_trie(node_t * current,node_t * parent){
  bool debug = true;
  string tail = current->get_tail();
  int tail_len = tail.length();
  if(debug)cerr<<"INDEX_TRIE:current node is tail "<<tail<<" with leaf status "<<current->get_is_leaf()<<endl;
  if(parent!=NULL ){
    for(int i=0;i<tail_len-1;++i){
      tree_pos_key_t tp_key;
      tp_key.first = tail[i];
      tp_key.second = tail[i+1];
      tree_pos_val_t tp_val;
      tp_val.first = current;
      tp_val.second = i;
      if(debug)cerr<<"INDEX_TRIE:Pair to add is "<<tp_key.first<<","<<tp_key.second<<":"<<tp_val.first<<","<<(int)tp_val.second<<endl;
      tpm[tp_key].push_back(tp_val);
    }
  }
  node_list_t nl = current->get_children();
  for(node_list_t::iterator it = nl.begin();it!=nl.end();++it){
    node_t * curr = *it;
    tree_pos_key_t tp_key;
    tp_key.first = tail[tail_len-1];
    tp_key.second = curr->get_val();
    tree_pos_val_t tp_val;
    tp_val.first = current;
    tp_val.second = tail_len-1;
    if(debug)cerr<<"INDEX_TRIE:Pair to add cross descendant is "<<tp_key.first<<","<<tp_key.second<<":"<<tp_val.first<<","<<(int)tp_val.second<<endl;
     tpm[tp_key].push_back(tp_val);
    if(debug)cerr<<"INDEX_TRIE:recursing on descendant\n";
    index_trie(curr,current);
  }
  if(debug)cerr<<"INDEX_TRIE:current node with tail "<<tail<<" is indexed now with leaf status "<<current->get_is_leaf()<<"\n";
  current->set_is_indexed(true);
  return;
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
  for(node_vector_map_t::iterator it1 = nvm.begin();
  it1!=nvm.end();it1++){
    string key = it1->first;
    node_vector_t vec = it1->second;
    for(node_vector_t::iterator it3 = vec.begin();
    it3!=vec.end();it3++){
      cerr<<"  Address: "<<*it3<<endl;
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
