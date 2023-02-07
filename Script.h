#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "Node.h"

struct _Script
{
	Node* node;

	//for inheritance
	//char* _className;
	void* _subClass;

	//virtual methods
	void (*deleteScript)(Script*);
	void (*ready)(Script*);
	//TODO enterTree()
	void (*process)(Script* thisScript, float delta);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiScript
{
	//Script* newScript(Node* node)
	Script* (*newScript)(Node* node);
};
//iScript : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiScriptInit();


#endif // SCRIPT_H_INCLUDED
