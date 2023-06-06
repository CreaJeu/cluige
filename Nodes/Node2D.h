#ifndef NODE2D_H_INCLUDED
#define NODE2D_H_INCLUDED

#include <stdbool.h>
#include "../Vector2.h"

typedef struct _Node2D Node2D;

struct _Node2D
{
	bool visible;
	Vector2 position;//local to parent; don't change directly, use methods

	//later : colorModulate, transform2D, rotation

	//private
	Vector2 _tmp_global_position;//don't change directly, use methods
	bool _local_position_changed;

	Node* _this_Node;
	void* _sub_class;

	//virtual methods
	void (*delete_Node)(Node*);
	void (*delete_Node2D)(Node*);
	void (*enter_tree_Node)(Node*);
	//in .c : void pre_process_Node(Node*);
	//in .c : void post_process_Node(Node*);
	//in .c : void on_loop_starting_Node2D(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiNode2D
{
	//Node2D* new_Node2D()
	Node2D* (*new_Node2D)();
	void (*show)(Node2D*);
	void (*hide)(Node2D*);
	void (*move_local)(Node2D*, Vector2);
	void (*set_local_position)(Node2D*, Vector2);
	//later : moveGlobal
};
//iNode2D : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiNode2D_init();


#endif // NODE2D_H_INCLUDED
