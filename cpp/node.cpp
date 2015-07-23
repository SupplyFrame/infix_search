#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<list>
#include<stack>
#include<vector>
#include<string.h>
#include<cstdlib>
#include<stdio.h>


using namespace std;
#include"node.hpp"
#include"search.hpp"

//int node2_t::current_node_index;
//vector<node2_t*> node2_t::node2_vec;

node_t::node_t(){}
node_t::~node_t(){}


node_t * node_t::make_node(int node_type){
  //cerr<<"Selected node type: "<<node_type<<endl;
  node_t * node = NULL;
  if(node_type==NODE_1){
    node = new node1_t();
  }else if(node_type==NODE_2){
    node = new node2_t();
    //node = node2_t::fetch_from_pool();
  }else{
    cerr<<"Invalid node type of "<<node_type<<".\n";
    exit(1);
  }
  return node;
}
node_list_t node1_t::get_children(){
  node_list_t node_list;
  for(node_map_t::iterator it = child_map.begin();it!=child_map.end();it++){
    node_list.push_back(it->second);
  }
  return node_list;
}

void node1_t::kill_children(){
  child_map.clear();
}

bool node1_t::child_exists(char val){
  return child_map.find(val)!=child_map.end(); 
}

node_t * node1_t::get_child(char val){
  if(!child_exists(val)) return NULL;
  else return child_map[val];
}

void node1_t::set_child(char val,node_t * node){
  child_map[val] = node;
}

bool node1_t::is_indexed(){
  return false;
}

char node1_t::get_val(){
  return val;
}

void node1_t::set_val(char val){
  this->val = val;
}

bool node1_t::get_is_leaf(){
  return is_leaf;
}

void node1_t::set_is_leaf(bool is_leaf){
  this->is_leaf = is_leaf;
}

string node1_t::get_tail(){
return tail;
}

void node1_t::set_tail(string tail){
  this->tail = tail;
}

node_t * node1_t::get_parent(){
  return parent;
}

void node1_t::set_parent(node_t * parent){
  this->parent = parent;
}

node1_t::node1_t(){
  val = '*';
  parent = NULL;
  tail="";
  is_leaf = false;
  ++search_t::node_count;
}

node1_t::~node1_t(){
}

node2_t::node2_t(){
  // most nodes only have two neighbors. parent and one child.
  // convention is 0th element is parent. 1th onward are children.
  total_neighbors = 2;
  neighbors = (node_t**)malloc(total_neighbors*sizeof(node_t*));
  for(int i=0;i<total_neighbors;++i){
    neighbors[i] = NULL;
  }
  // most nodes will only need three bytes of information
  // 0th byte is meta data, which we can extract by bit shifting
  // 1st byte is the node character value
  // 2nd byte is the null character as it will not have a tail
  // if a tail is needed, then realloc this array
  data = (char*)malloc(3);
  data[METADATA_INDEX] =  (1 << BIT_INDEX_RESERVED);
  data[CHARVAL_INDEX] = '*';
  data[TAIL_INDEX] = EOL;
  if(data[TAIL_INDEX]==data[METADATA_INDEX]) cerr<<"Equal!\n";
  //cerr<<"Constructed a node2 data len is "<<strlen(data)<<"\n";
  ++search_t::node_count;
}

node2_t::~node2_t(){
  delete[] neighbors;
  delete[] data;
}

node_list_t node2_t::get_children(){
  node_list_t nl;
  for(int i=1;i<total_neighbors;++i){
    if(neighbors[i]!=NULL) nl.push_back(neighbors[i]);
  }
  return nl;
}

bool node2_t::child_exists(char val){
  bool found = false;
  int i=1;
  while(i<total_neighbors && !found){
    if(neighbors[i]!=NULL && neighbors[i]->get_val()==val) found = true;
    ++i;
  }
  return found;
}

void node2_t::set_child(char val,node_t * node){
  bool found = false;
  int i=1;
  while(i<total_neighbors && !found){
    if(neighbors[i]==NULL){
      found = true;
      neighbors[i] = node;
      //cerr<<" using existing neighbor slot at "<<i<<endl;
    }else if(neighbors[i]->get_val()==val){
      cerr<<" duplicate child, ignoring set request\n";
      return;
    }
    ++i;
  }
  if(!found){
    ++total_neighbors;
    node_t ** status = NULL;
    //cerr<<" need to expand the size of neighbors array\n";
    status = (node_t **)realloc(neighbors,total_neighbors*sizeof(node_t *));
    if(status!=NULL){
      //cerr<<" expanded the size of neighbors array\n";
      neighbors = status;
    }else{
      cerr<<"Problem expanding neighbors array\n";
      exit(1);
    }
    neighbors[total_neighbors-1] = node;
  }
}

void node2_t::kill_children(){
  total_neighbors = 2;
  node_t ** status = NULL;
  status = (node_t **)realloc(neighbors,total_neighbors*sizeof(node_t *));
  if(status!=NULL){
    //cerr<<" expanded the size of neighbors array\n";
    neighbors = status;
  }else{
    cerr<<"Problem shrinking neighbors array in kill_children\n";
    exit(1);
  }
  neighbors[total_neighbors-1] = NULL;
}

char node2_t::get_val(){
  return data[CHARVAL_INDEX];
}
void node2_t::set_val(char val){
  data[CHARVAL_INDEX] = val;
}

bool node2_t::get_is_leaf(){
  int metadata = data[METADATA_INDEX];
  return (metadata>>BIT_INDEX_IS_LEAF)&1;

}
void node2_t::set_is_leaf(bool is_leaf){
  data[METADATA_INDEX] |= 1 << BIT_INDEX_IS_LEAF;
}

string node2_t::get_tail(){
  string str(data+TAIL_INDEX);
  return str;
}

void node2_t::set_tail(string tail){
  int cur_size = strlen(data)+1;
  int new_size = 3+tail.length();
  //cerr<<"tail len "<<tail.length()<<" current and new size is "<<cur_size<<","<<new_size<<endl;
  if(cur_size!=new_size){
    char * status = NULL;
    status = (char *)realloc(data,new_size);
    if(status!=NULL){
      //cerr<<"Reallocated "<<this<<" to size "<<new_size<<endl;
      data = status;
    }else{
      cerr<<"Failed to reallocated to size "<<new_size<<endl;
      exit(1);
    }
  }
  //cerr<<"Inserting "<<tail.data()<<" into position "<<TAIL_INDEX<<endl;
  strcpy(data+TAIL_INDEX,tail.data());
  //cerr<<"After tail set, data is size "<<strlen(data)<<" with contents "<<data<<endl;

}

node_t * node2_t::get_child(char val){
  node_t * node=NULL;
  int i=1;
  bool found =false;
  while(i<total_neighbors && !found){
    if(neighbors[i]!=NULL && neighbors[i]->get_val()==val) {
      node = neighbors[i];
      found = true;
    }
    ++i;
  }
  return node;
}

bool node2_t::is_indexed(){
  return false;
}

node_t * node2_t::get_parent(){
  return neighbors[0];
}

void node2_t::set_parent(node_t * parent){
  neighbors[0] = parent;
}


//node2_t * node2_t::fetch_from_pool(){
//  return new node2_t();
//  if(node2_vec.size()==0){
//    current_node_index=0;
//    cerr<<"Allocating static array for node objects\n";
//    node2_vec = vector<node2_t * >(100,NULL);
//    for(int i=0;i<node2_vec.size();++i) node2_vec[i] = new node2_t();
//    cerr<<"Size is now "<<node2_vec.size()<<endl;
//  }
//  if(current_node_index == node2_vec.size()){
//    //cerr<<"Size is "<<node2_vec.size()<<endl;
//    //cerr<<"Expanding pool of nodes by one\n";
//    node2_vec.push_back(new node2_t());
//  }
//  return node2_vec[current_node_index++];
//}

int node2_t::get_token(int token_id,char buf[]){
  char * data;
  bool debug = false;
  int i=0;
  if(debug)cerr<<"Data is:"<<data<<":\n";
  int offset=-1;
  char * pch = strchr(data,DELIM);
  int cur_pos =0,last_pos = 0 ;
  while(pch!=NULL){
    cur_pos = pch-data;
    if(debug)cerr<<"Cur pos "<<cur_pos<<endl;
    if(token_id==i){
      offset = last_pos;
    }
    if(debug)cerr<<"Token "<<i<<" starts at:"<<last_pos<<":"<<endl;
    last_pos = cur_pos+1;
    pch = strchr(pch+1,DELIM);
    ++i;
  }
  if(offset<0)throw "Negative offset for node data";
  if(debug)cerr<<"Offset is "<<offset<<endl;
  char cur_char=EOL;
  i=0;
  do{
    cur_char = data[offset+i]; 
    if(debug)cerr<<"i:"<<i<<"cur_char: "<<cur_char<<endl;
    buf[i] = cur_char;
    ++i;
  }while(cur_char!=DELIM && cur_char!=EOL);
  buf[i-1] = EOL;
  return strlen(buf);
}

void node2_t::set_token(int token_id,char buf[]){
  char * data;
  bool debug = true;
  int data_len = strlen(data);
  int input_len = strlen(buf);
  int i=0;
  if(debug)cerr<<"Data was "<<data<<endl;
  if(debug)cerr<<"Data and input is of lengths:"<<data_len<<" and "<<input_len<<endl<<"\n";
  int offset=-1;
  char * pch = strchr(data,DELIM);
  int cur_pos =0,last_pos = 0 ;
  while(pch!=NULL){
    cur_pos = pch-data;
    if(debug)cerr<<"Cur pos "<<cur_pos<<endl;
    if(token_id==i){
      offset = last_pos;
    }
    if(debug)cerr<<"Token "<<i<<" starts at:"<<last_pos<<":"<<endl;
    last_pos = cur_pos+1;
    pch = strchr(pch+1,DELIM);
    ++i;
  }
  if(offset<0)throw "Negative offset for node data";
  if(debug)cerr<<"Offset is "<<offset<<endl;
  char old_data[100];
  int new_size = data_len+input_len+1;
  strcpy(old_data,data);
  delete[]data;
  data = (char*)malloc(new_size);
  int data_index=0;
  int new_index = 0;
  int buf_index = 0;
  // copy from the old data
  for(data_index=0;data_index<offset;++data_index,++new_index){
    data[new_index] = old_data[data_index];
  }
  // insert at the proper place 
  do{
    data[new_index] = buf[buf_index];
    ++new_index;
    ++buf_index;
  }while(buf[buf_index]!=EOL);
  // now we should skip any characters before the next delimiter
  while(old_data[data_index]!=DELIM) ++data_index;
  // now copy the rest of the stuff from the old data
  do{
    data[new_index] = old_data[data_index];
    ++new_index;
    ++data_index;
  }while(old_data[data_index]!=EOL);
  data[new_index]=EOL;
  if(debug)cerr<<"Data is now "<<data<<endl;
  
  //return strlen(buf);
}

