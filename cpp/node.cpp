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

node_t * node_t::head;

node_t * node_t::make_node(){
  return new node_t();
}
node_list_t node_t::get_children(){
  node_list_t node_list;
  for(node_map_t::iterator it = child_map.begin();it!=child_map.end();it++){
    node_list.push_back(it->second);
  }
  return node_list;
}

bool node_t::child_exists(char val){
  return child_map.find(val)!=child_map.end(); 
}

node_t * node_t::get_child(char val){
  if(!child_exists(val)) return NULL;
  else return child_map[val];
}

void node_t::set_child(char val,node_t * node){
  child_map[val] = node;
}

char node_t::get_val(){
  return val;
}

void node_t::set_val(char val){
  this->val = val;
}

bool node_t::get_is_leaf(){
  return is_leaf;
}

void node_t::set_is_leaf(bool is_leaf){
  this->is_leaf = is_leaf;
}

string node_t::get_tail(){
return tail;
}

void node_t::set_tail(string tail){
  this->tail = tail;
}

node_t * node_t::get_parent(){
  return parent;
}

void node_t::set_parent(node_t * parent){
  this->parent = parent;
}

node_t::node_t(){
  val = '*';
  parent = NULL;
  tail="";
  is_leaf = false;
  ++search_t::node_count;
}


char * data;
node2_t::node2_t(){
  parent = NULL;
  data = NULL;
  ++search_t::node_count;
  data = new char[20];
  // token 0: pointer to its next sibling (0 or 8 bytes)
  // tab delimiter (1 byte)
  // token 1: pointer to its first child (0 or 8 bytes)
  // tab delimiter (1 byte)
  // token 2: is_leaf is a boolean (0 or 1 byte)
  // tab delimiter (1 byte)
  // token 3: val is a char (0 or 1 byte)
  // tab delimiter (1 byte)
  // token 4: tail is a null terminated string (strlen + 1 byte)
  int i=0;
  data[i++]=DELIM;
  data[i++]='g';
  data[i++]=DELIM;
  data[i++]='a';
  data[i++]='r';
  data[i++]=DELIM;
  data[i++]=DELIM;
  data[i++] = 'h';
  data[i++] = DELIM;
  data[i++] = EOL;
}

int node2_t::get_token(int token_id,char buf[]){
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
  data = new char[new_size];
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

void node2_t::set_parent(node2_t * node){
  this->parent =  node;
}
