#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <stdbool.h>

typedef struct _Node Node;
typedef struct _Script Script;

struct _Node
{
	// ... position/transform
	Node* parent;
	Node* next_sibling;
	Node* children;
	Script* script;
	char* name;//must not point to the stack, see set_name()
	bool active;

	//TODO : get child by name, get node by path, _draw, queue_free

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
    //void initZero(Node* node)
//	void (*initZero)(Node*);// no : would encourage not freeable nodes on the stack

	//Node* new_Node()
	Node* (*new_Node)();

	//void delete_Node(Node*)
	//void (*delete_Node)(Node*); //see struct _Node (virtual method)

	//int get_index(Node*)
	int (*get_index)(const Node*);

	//int get_depth(Node*)
	int (*get_depth)(const Node*);

	//use set_name(n, "bob") instead of n->name = "bob"
	//previous name is automatically freed
	//void set_name(Node*, char* new_name)
	void (*set_name)(Node*, const char* new_name);

	//asserts that wanted child doesn't have already a parent
	//void add_child(Node* p, Node* c)
	void (*add_child)(Node* p, Node* c);

	//void print_tree_pretty(Node*)
	void (*print_tree_pretty)(const Node*);

	//TODO removeChild()
	//TODO queueFree()
};
//iNode : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiNode_init();


#endif // NODE_H_INCLUDED
