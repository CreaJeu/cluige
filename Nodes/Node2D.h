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
	Vector2 _tmpGlobalPosition;//don't change directly, use methods
	bool _localPositionChanged;

	Node* _thisNode;
	void* _subClass;

	//virtual methods
	void (*deleteNode)(Node*);
	void (*deleteNode2D)(Node*);
	//in .c : void preProcessNode(Node*);
	//in .c : void postProcessNode(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiNode2D
{
	//Node2D* newNode2D()
	Node2D* (*newNode2D)();
	void (*show)(Node2D*);
	void (*hide)(Node2D*);
	void (*moveLocal)(Node2D*, Vector2);
	//later : moveGlobal
};
//iNode2D : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiNode2DInit();


#endif // NODE2D_H_INCLUDED
