
//typedef map<char,node_list_t> node_list_map_t;
//typedef map<char,node_list_map_t> node_list_map_map_t;
typedef map<string,node_vector_t> node_vector_map_t;

class search_t{
private:
  string algorithm,dict_file,query_file;
  node_t * head;
  node_t * traverse;
  //node_list_map_map_t nlmm;
  node_vector_map_t nvm;
  void add_to_lookup(char parent_val,node_t * child_node);
  void add(string word);
  void traverse_up(node_t * node,stack<char> & up_stack);
  void traverse_down_util(node_t * node, char path[],int pathLen,list<string> & suffixes);
  void traverse_down(node_t * node,list<string> & suffixes,string orig_key);
  void find_path(string key);
  bool find_path_util(node_t * node,string sub_key,node_t * start,string orig_key,bool & exact_match);
  void read_query(const char * query_file,vector<string> & keys);
  void print_lookup();
  void find_in_file(string key,const char * dict_file);
  void read_input(const char * dict_file);
public:
  static int node_count;
  search_t(int argc,char * arg[]);
  void run(bool simple);
};
