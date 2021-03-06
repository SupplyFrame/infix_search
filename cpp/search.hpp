#include<stack>
struct tree_pos_key2_t{
  char a;
  char b;
};
typedef pair<char,char> tree_pos_key_t;
typedef pair<node_t*,char> tree_pos_val_t;
typedef vector<node_t *> node_vector_t;
typedef map<string,node_vector_t> node_vector_map_t;
typedef vector<tree_pos_val_t> tree_pos_val_vector_t;
typedef map<tree_pos_key_t,tree_pos_val_vector_t> tree_pos_map_t;
typedef map<string,pair<off_t,int> > row_lookup_map_t;

class search_t{
private:
  static const int node_type = node1_t::NODE_2;
  row_lookup_map_t row_lookup_map;
  //string algorithm,dict_file,query_file;
  FILE * pFile;
  node_t * head;
  node_t * traverse;
  //node_list_map_map_t nlmm;
  node_vector_map_t nvm;
  tree_pos_map_t tpm;
  char * row_buffer;
  // recursive function to index the trie
  void index_trie(node_t * current,node_t * parent);
  void add_to_lookup(char parent_val,node_t * child_node);
  void add(string word);
  void traverse_up(tree_pos_val_t tpv,stack<char> & up_stack);
  void traverse_down_util(node_t * node, int tailindex,char path[],int pathLen,list<string> & suffixes);
  void traverse_down(node_t * node,list<string> & suffixes,string orig_key);
  bool find_path_util(node_t * node,int charindex,string sub_key,tree_pos_val_t start,string orig_key,bool & exact_match,vector<string> & found_keys);
  void read_query(const char * query_file,vector<string> & keys);
  void print_lookup();
  void find_in_file(string key,const char * dict_file);
public:
  void read_input(const char * dict_file);
  void find_path(string key,vector<string> & found);
  string extract_row(string key);
  static int node_count;
  search_t();
  ~search_t();
  void run(bool simple,string algorithm,string dict_file, string query_file);
};
