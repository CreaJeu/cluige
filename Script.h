#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "Node.h"

struct _Script
{
	Node* node;

	//for inheritance
	//char* _class_name;
	void* _sub_class;

	//virtual methods
	void (*delete_Script)(Script*);
	void (*enter_tree)(Script*);
	void (*ready)(Script*);
	void (*exit_tree)(Script*);
	void (*process)(Script* this_Script, float delta);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiScript
{
	//Script* new_Script(Node* node)
	Script* (*new_Script)(Node* node);
};
//iScript : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiScript_init();


#endif // SCRIPT_H_INCLUDED
