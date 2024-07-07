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
	bool active;

	//private
	bool _already_entered_tree;

	//for inheritance
	char* _class_name;
	void* _sub_class;

	//virtual methods
	void (*delete_Node)(Node*);
	void (*enter_tree)(Node*);
	void (*on_loop_starting)(Node*);
	void (*pre_process_Node)(Node*);
	void (*process_Node)(Node*);
	void (*post_process_Node)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiNode
{
	// ~public static
	//Dico<String, NodeFactory*>
	// class_name -> factory*
	SortedDictionary node_factories;

	// ~private static
	NodeFactory _Node_factory;

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
	char* (*get_path_mallocing)(Node* node);

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
	void (*_do_all_queue_free)();
};
//iNode : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiNode_init();


#endif // NODE_H_INCLUDED
