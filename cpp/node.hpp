struct node_t;
struct node2_t;

typedef list<node_t *> node_list_t;
typedef map<char,node_t *> node_map_t;


class node_t{
public:
  static const int NODE_1 = 0;
  static const int NODE_2 = 1;
  // factory method 
  static node_t * make_node(int node_type);
  virtual void kill_children()=0;
  virtual node_list_t get_children()=0;
  virtual bool child_exists(char val)=0;
  virtual node_t * get_child(char val)=0;
  virtual void set_child(char val,node_t * node)=0;
  virtual bool get_is_indexed()=0;
  virtual void set_is_indexed(bool is_indexed)=0;
  virtual char get_val()=0;
  virtual void set_val(char val)=0;
  virtual bool get_is_leaf()=0;
  virtual void set_is_leaf(bool is_leaf)=0;
  virtual string get_tail()=0;
  virtual void set_tail(string tail)=0;
  virtual node_t * get_parent()=0;
  virtual void set_parent(node_t * parent)=0;
protected:
  node_t();
  virtual ~node_t();
};

class node1_t:public node_t{
private:
  char val;
  bool is_leaf;
  bool is_indexed;
  node_t * parent;
  string tail;
  node_map_t child_map;
  static node1_t ** node1_t_arr;
  static node1_t * head;
public:
  node1_t();
  ~node1_t();
  node_list_t get_children();
  void kill_children();
  bool child_exists(char val);
  node_t * get_child(char val);
  void set_child(char val,node_t * node);
  char get_val();
  void set_val(char val);
  bool get_is_leaf();
  void set_is_leaf(bool is_leaf);
  string get_tail();
  void set_tail(string tail);
  bool get_is_indexed();
  void set_is_indexed(bool is_indexed);
  node_t * get_parent();
  void set_parent(node_t * parent);
};
class node2_t:public node_t{
private:
  //static vector<node2_t*> node2_vec;
  //static int current_node_index;
  static const int METADATA_INDEX=0;
  static const int TOTAL_NEIGHBORS_INDEX=1;
  static const int TAIL_INDEX=2;
  // bit indexes start from the right most position
  static const int BIT_INDEX_IS_LEAF=0;
  static const int BIT_INDEX_IS_INDEXED=1;
  static const int BIT_INDEX_RESERVED=7;
  //vector<node_t * > neighbors;
  node_t ** neighbors;
  //int total_neighbors;
  //string other_data;
  char * data;
  // we store things as an array of bytes because some fields are optional
  // and we don't want to suffer memory bloat from struct padding.
  // 
  // data is a null terminated, tab delimited variable length array of 
  // characters.  Tabs separate tokens. If a token is 0 length, 
  // it is not defined.
  // 
  // token 0: pointer to its next sibling (0 or 8 bytes)
  // tab delimiter (1 byte)
  // token 1: pointer to its first child (0 or 8 bytes)
  // tab delimiter (1 byte)
  // token 2: is_leaf is a boolean (0 or 1 byte)
  // tab delimiter (1 byte)
  // token 3: val is a char (0 or 1 byte)
  // tab delimiter (1 byte)
  // token 4: tail is a null terminated string (strlen + 1 byte)
  // tab delimiter (1 byte)
  //char * data;
public:
  static node2_t * fetch_from_pool();
  node2_t();
  ~node2_t();
  // returns strlen
  int get_token(int token_id,char buf[]);
  void set_token(int token_id,char buf[]);
  static const int TOKEN_SIBLING_PTR=0;
  static const int TOKEN_CHILD_PTR=1;
  static const int TOKEN_IS_LEAF = 2;
  static const int TOKEN_VAL = 3;
  static const int TOKEN_TAIL = 4;
  static const char DELIM = '*';
  static const char EOL = '\0';
  node_list_t get_children();
  void kill_children();
  bool child_exists(char val);
  node_t * get_child(char val);
  bool get_is_indexed();
  void set_is_indexed(bool is_indexed);
  void set_child(char val,node_t * node);
  char get_val();
  void set_val(char val);
  bool get_is_leaf();
  void set_is_leaf(bool is_leaf);
  string get_tail();
  void set_tail(string tail);
  node_t * get_parent();
  void set_parent(node_t * parent);
};
