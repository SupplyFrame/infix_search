struct node_t;
struct node2_t;
typedef list<node_t *> node_list_t;
typedef list<node2_t *> node2_list_t;
typedef map<char,node_t *> node_map_t;

class node_t{
private:
  char val;
  bool is_leaf;
  node_t * parent;
  string tail;
  node_map_t child_map;
public:
  node_list_t get_children();
  bool child_exists(char val);
  node_t * get_child(char val);
  void set_child(char val,node_t * node);
  char get_val();
  void set_val(char val);
  bool get_is_leaf();
  void set_is_leaf(bool is_leaf);
  string get_tail();
  void set_tail(string tail);
  node_t * get_parent();
  void set_parent(node_t * parent);
  node_t();
};

class node2_t{
private:
  // a parent is always present
  node2_t * parent;
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
  char * data;
public:
  node2_list_t get_children();
  bool child_exists(char val);
  node2_t * get_child(char val);
  void set_child(char val,node2_t * node);
  char get_val();
  void set_val(char val);
  bool get_is_leaf();
  void set_is_leaf(bool is_leaf);
  string get_tail();
  void set_tail(string tail);
  node2_t * get_parent();
  void set_parent(node2_t * parent);
  node2_t();
};

