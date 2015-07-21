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
  data = new char[5];
  for(int i=0;i<4;++i) data[i] = '\t';
  data[4] = '\0';
}

void node2_t::set_parent(node2_t * node){
  this->parent =  node;
}
