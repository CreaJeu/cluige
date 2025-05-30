#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <stdbool.h>

//for node_factories for godot import
#include "SortedDictionary.h"

typedef struct _Node Node;
typedef struct _Script Script;
typedef struct _NodeFactory NodeFactory;


struct _NodeFactory
{
    Node* (*instanciate)(const SortedDictionary* parsed_params);
};

struct _Node
{
	// position/transform : in Node2D
	Node* parent;
	Node* next_sibling;
	Node* children;
	Script* script;
	char* name;//must not point to the stack, see set_name()
//	bool active;
	int process_priority;//lower is processed sooner, default = 0

	//private
	bool _already_entered_tree;
	bool _marked_for_queue_free;
//	struct
//	{
//		Node* parent;
//		Node* next_sibling;
//		Node* children;
//		Script* script;
//		char* name;//must not point to the stack, see set_name()
////		bool active;
//		int process_priority;//lower is processed sooner, default = 0
//		bool already_entered_tree;
//		bool marked_for_queue_free;
//	} _state_changes;//see old_baked/new_baked in Node2D for example if needed

	//overview of the game loop :
	//all process() functions all called
	//	they read/write only raw data
	//then all bake()  functions all called
	//	they cook raw to baked data
	//		(and keep a backup of their previous values from frame N-1)
	//then all erase(), then all draw()
	//	they only read, and only from baked data

	//for inheritance
	char* _class_name;
	void* _sub_class;

	//virtual methods
	void (*delete_Node)(Node*);
	void (*enter_tree)(Node*);//called at each tree entering
	void (*ready)(Node*);//called after enter_tree, but only on the 1st entering ever
	void (*exit_tree)(Node*);//called just before remove_child logic
//	void (*on_loop_starting)(Node*);
	void (*erase)(Node*);//abstract (null)
	void (*process)(Node*);
	void (*bake)(Node*);//abstract (null)
	void (*draw)(Node*);//abstract (null)
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiNode
{
	int _MAX_NAME_LENGTH;
	// ~public static
	//Dico<String, NodeFactory*>
	// class_name -> factory*
	SortedDictionary node_factories;

	// ~private static
	NodeFactory _Node_factory;

	// ~private static
	Deque _just_removed_nodes;

	//void (*initZero)(Node*);// no : would encourage not freeable nodes on the stack

	//Node* new_Node()
	Node* (*new_Node)();

	//void (*delete_Node)(Node*); // virtual, see struct _Node

	// read

	//int get_index(Node*)
	int (*get_index)(const Node*);

	//int get_depth(Node*)
	int (*get_depth)(const Node*);

	//get the child at idx
	Node* (*get_child)(const Node* ths_node, int idx);

	//get the number of children of the current node
	int (*get_child_count)(const Node* ths_node);

	//get a node based on the relative or absolute path
	//examples: /root/child_test1/
	//          test1/child_test1/
	//          test1/child_test1
	//          ../sibling3
	Node* (*get_node)(Node* ths_node, const char* node_path);

	bool (*is_ancestor_of)(Node* ths_node, Node* potential_ancestor);
	bool (*is_higher_than)(Node* ths_node, Node* node);

	//the returned string is malloced, up to the user to free it later
	char* (*get_path_mallocing)(const Node* node);

	//void print_tree_pretty(Node*)
	void (*print_tree_pretty)(const Node*);

	// write

	//user must call this function like instead of n->name = "bob"
	//previous name is automatically freed
	//void set_name(Node*, char* new_name)
	void (*set_name)(Node*, const char* new_name);

	//asserts that wanted child doesn't have already a parent
	//void add_child(Node* p, Node* c)
	void (*add_child)(Node* p, Node* c);

	//The user have to save or free the Node manually
	void (*remove_child)(Node* ths_node, Node* child);

	void (*queue_free)(Node* node);

	//Only for cluige internal logic
	void (*_do_all_queue_free_early_step)();//remove from tree
	void (*_do_all_queue_free_late_step)();//free, separated to be called between refresh() and sleep()

	void (*register_NodeFactory)(const char* key, NodeFactory* factory);
};
//iNode : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiNode_init();


#endif // NODE_H_INCLUDED
